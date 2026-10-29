"""Resolve user-supplied relative paths against the checkout, never the shell cwd."""
import os
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DEFAULT_INSTALL_DIR = Path("runtime/server")


def project_root():
    # The Android wrapper supplies a temporary ASCII drive alias. Keep that
    # spelling for legacy tools, but only after checking its real destination.
    alias = os.environ.get("NRB_PROJECT_ROOT")
    if alias:
        candidate = Path(os.path.abspath(alias))
        if candidate.resolve() != REPO:
            raise ValueError("NRB_PROJECT_ROOT does not point to this checkout")
        return candidate
    return REPO


def project_path(value):
    path = Path(value)
    if (path.drive or path.root) and not path.is_absolute():
        raise ValueError("Use a project-relative path or a fully qualified absolute path")
    return Path(os.path.abspath(project_root() / path))
