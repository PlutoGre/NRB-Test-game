"""Build the administration name resource from the actual published client tables."""
import argparse
import hashlib
import json
from pathlib import Path
import zlib

ROOT = Path(__file__).resolve().parents[2]


def table(name):
    data = (ROOT / "src/server/ResourcesAndroid_Encrypt" / name).read_bytes()
    if data[:2] == b"\xef\xfe":
        key = b"youaiABC"
        data = bytes(byte ^ key[index % len(key)] for index, byte in enumerate(data[2:]))
        data = zlib.decompress(data[1:])
    return [line.split() for line in data.decode("utf-8-sig").splitlines()[1:] if line.strip()]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    names = {}
    for kind, file, column in (("card", "Disciples.txt", 1), ("equip", "Equips.txt", 1),
                               ("skill", "Skills.txt", 1), ("tool", "Tools.txt", 1),
                               ("piece", "SkillBooks.txt", 2)):
        rows = table(file)
        for row in rows:
            if row and row[0].isdigit():
                names[kind + ":" + row[0]] = row[column]
        print("Published item names:", kind, sum(key.startswith(kind + ":") for key in names))
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "admin-names.json").write_text(json.dumps(names, ensure_ascii=False), encoding="utf-8")
    # The default credential is explicitly requested for this personal test project.
    # Store only its digest in the Java resource; never log passwords or session tokens.
    (args.output / "admin-auth.properties").write_text(
        "username=op_root\npasswordSha256=" + hashlib.sha256(b"hy1212").hexdigest() + "\n", encoding="ascii")


if __name__ == "__main__":
    main()
