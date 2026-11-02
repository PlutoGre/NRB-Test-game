"""Precompress public Web build files once and validate browser download caches."""
import gzip
import hashlib
from pathlib import Path
import tempfile
import threading


def accepts_gzip(value):
    """Respect explicit gzip exclusions, including when a wildcard is present."""
    qualities = {}
    for item in value.split(","):
        parts = [part.strip() for part in item.split(";")]
        quality = 1.0
        for parameter in parts[1:]:
            name, _, raw = parameter.partition("=")
            if name.lower() == "q":
                try:
                    quality = float(raw)
                except ValueError:
                    quality = 0.0
                if not 0 <= quality <= 1:
                    quality = 0.0
        qualities[parts[0].lower()] = quality
    return qualities.get("gzip", qualities.get("*", 0.0)) > 0


def unchanged(value, etag):
    """If-None-Match uses weak comparison for GET, across encoded variants."""
    expected = etag.removeprefix("W/")
    return any(tag.strip() == "*" or tag.strip().removeprefix("W/") == expected
               for tag in value.split(","))


class WebAssets:
    """Keep compressed files on disk so concurrent players do not recompress them."""

    def __init__(self, build, cache, names):
        self.build, self.cache = Path(build), Path(cache)
        self.names = frozenset(names)
        self.entries = {}
        self.lock = threading.Lock()
        try:
            self.cache.mkdir(parents=True, exist_ok=True)
        except OSError as error:
            print("Web asset compression unavailable; serving original files: " + str(error), flush=True)
            self.cache = None
        entries = [self.get(name) for name in sorted(self.names)]
        self.summary = {"originalBytes": sum(entry["size"] for entry in entries),
                        "gzipBytes": sum(entry["gzipSize"] for entry in entries)}
        print("Web bundle: original {originalBytes} bytes; gzip transfer estimate {gzipBytes} bytes."
              .format(**self.summary), flush=True)

    @staticmethod
    def signature(path):
        info = path.stat()
        return info.st_size, info.st_mtime_ns, info.st_ctime_ns

    def compress(self, target, digest):
        """Publish completed gzip files atomically, preserving an existing good cache."""
        compressed = self.cache / (digest + "-gzip6.gz")
        temporary = None
        try:
            if not compressed.is_file() or compressed.stat().st_size == 0:
                print("Preparing compressed Web asset: " + target.name, flush=True)
                with tempfile.NamedTemporaryFile(dir=self.cache, prefix=".web-", suffix=".tmp", delete=False) as stream:
                    temporary = Path(stream.name)
                    # Strip source paths/timestamps to make the cached encoding stable.
                    with target.open("rb") as source, gzip.GzipFile(
                            filename="", mode="wb", compresslevel=6, fileobj=stream, mtime=0) as encoded:
                        actual = hashlib.sha256()
                        while block := source.read(1024 * 1024):
                            actual.update(block)
                            encoded.write(block)
                if actual.hexdigest() != digest:
                    raise ValueError("Web build changed during compression; finish the build and restart the gateway")
                temporary.replace(compressed)
            return compressed
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)

    def get(self, name):
        """Refresh hashes only when a public build file changes, never per downloader."""
        if name not in self.names:
            raise ValueError("Unknown Web asset")
        with self.lock:
            target = self.build / name
            signature = self.signature(target)
            previous = self.entries.get(name)
            if previous and previous["signature"] == signature:
                return previous
            digest = hashlib.sha256()
            with target.open("rb") as source:
                while block := source.read(1024 * 1024):
                    digest.update(block)
            # Weak validators compare decoded content; Vary separates encodings.
            entry = {"path": target, "size": signature[0], "signature": signature,
                     "etag": 'W/"' + digest.hexdigest() + '"', "gzipPath": None, "gzipSize": signature[0]}
            if self.cache is not None and signature[0] >= 1024:
                try:
                    compressed = self.compress(target, digest.hexdigest())
                    size = compressed.stat().st_size
                    if size < signature[0]:
                        entry.update(gzipPath=compressed, gzipSize=size)
                except OSError as error:
                    print("Web asset compression failed for " + name + "; serving original: " + str(error), flush=True)
            if self.signature(target) != signature:
                raise ValueError("Web build changed while preparing downloads; finish the build and restart the gateway")
            self.entries[name] = entry
            return entry
