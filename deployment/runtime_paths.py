"""Owned Windows DOS-device aliases for legacy server binaries, without moving data."""
from contextlib import contextmanager
import ctypes
from ctypes import wintypes


def api():
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.QueryDosDeviceW.argtypes = [wintypes.LPCWSTR, wintypes.LPWSTR, wintypes.DWORD]
    kernel.QueryDosDeviceW.restype = wintypes.DWORD
    kernel.DefineDosDeviceW.argtypes = [wintypes.DWORD, wintypes.LPCWSTR, wintypes.LPCWSTR]
    kernel.DefineDosDeviceW.restype = wintypes.BOOL
    kernel.GetLogicalDrives.restype = wintypes.DWORD
    kernel.CreateMutexW.argtypes = [ctypes.c_void_p, wintypes.BOOL, wintypes.LPCWSTR]
    kernel.CreateMutexW.restype = wintypes.HANDLE
    kernel.WaitForSingleObject.argtypes = [wintypes.HANDLE, wintypes.DWORD]
    kernel.ReleaseMutex.argtypes = [wintypes.HANDLE]
    kernel.CloseHandle.argtypes = [wintypes.HANDLE]
    return kernel


def target(kernel, device):
    buffer = ctypes.create_unicode_buffer(32768)
    if kernel.QueryDosDeviceW(device, buffer, len(buffer)):
        return buffer.value
    error = ctypes.get_last_error()
    if error not in (2, 3):
        raise ctypes.WinError(error)
    return None


@contextmanager
def drive_lock(kernel):
    # Also used by Build-Android.ps1 when allocating its temporary aliases.
    mutex = kernel.CreateMutexW(None, False, "Local\\NRB-BuildDriveAllocation")
    if not mutex:
        raise ctypes.WinError(ctypes.get_last_error())
    try:
        waited = kernel.WaitForSingleObject(mutex, 30000)
        if waited not in (0, 0x80):  # WAIT_OBJECT_0 or an abandoned, now owned mutex.
            raise RuntimeError("Cannot acquire the runtime path allocation lock")
        try:
            yield
        finally:
            kernel.ReleaseMutex(mutex)
    finally:
        kernel.CloseHandle(mutex)


def alias_target(root):
    root = str(root.resolve()).rstrip("\\")
    if root.startswith("\\\\?\\UNC\\"):
        root = "\\\\" + root[8:]
    elif root.startswith("\\\\?\\"):
        root = root[4:]
    expected = "\\??\\UNC\\" + root[2:] if root.startswith("\\\\") else "\\??\\" + root
    return root, expected


def ensure_alias(root, previous, active):
    kernel = api()
    root, expected = alias_target(root)
    with drive_lock(kernel):
        device = previous.get("device", "") if previous else ""
        valid = len(device) == 2 and "A" <= device[0] <= "Z" and device[1] == ":"
        if (previous and valid and isinstance(previous.get("target"), str)
                and previous["target"].casefold() == expected.casefold()):
            device = previous["device"]
            current = target(kernel, device)
            if current and current.casefold() == expected.casefold():
                return previous
            if current is None and not kernel.GetLogicalDrives() & (1 << (ord(device[0]) - 65)):
                # Reboot removes local DOS devices. Restore the saved letter only
                # if it is still free, keeping a running server's filenames valid.
                if kernel.DefineDosDeviceW(0, device, root):
                    current = target(kernel, device)
                    if not current or current.casefold() != expected.casefold():
                        kernel.DefineDosDeviceW(6, device, root)
                        raise RuntimeError("Cannot restore the runtime path mapping")
                    return {"device": device, "target": current}
        if active and previous:
            raise RuntimeError("Runtime path mapping changed while services are running; inspect the mapping before restarting")
        # An older deployment without a pathAlias may already be running from
        # its physical path. Adding an alias only changes new client commands.
        occupied = kernel.GetLogicalDrives()
        for number in range(25, 2, -1):
            device = chr(65 + number) + ":"
            if occupied & (1 << number) or target(kernel, device) is not None:
                continue
            if not kernel.DefineDosDeviceW(0, device, root):
                raise ctypes.WinError(ctypes.get_last_error())
            current = target(kernel, device)
            if not current or current.casefold() != expected.casefold():
                kernel.DefineDosDeviceW(6, device, root)
                raise RuntimeError("Cannot record the runtime path mapping")
            return {"device": device, "target": current}
    raise RuntimeError("No free drive letter is available for legacy server tools")


def release_alias(root, record):
    _, expected = alias_target(root)
    device = record.get("device", "")
    if (len(device) != 2 or not "A" <= device[0] <= "Z" or device[1] != ":"
            or not isinstance(record.get("target"), str) or record["target"].casefold() != expected.casefold()):
        raise RuntimeError("Runtime drive belongs to a different directory; no mapping was removed")
    kernel = api()
    with drive_lock(kernel):
        current = target(kernel, record["device"])
        if current is None:
            return
        if current != record["target"]:
            raise RuntimeError("Runtime drive identity changed; no foreign mapping was removed")
        # RAW_TARGET_PATH | REMOVE_DEFINITION | EXACT_MATCH_ON_REMOVE.
        if not kernel.DefineDosDeviceW(7, record["device"], record["target"]):
            raise ctypes.WinError(ctypes.get_last_error())
