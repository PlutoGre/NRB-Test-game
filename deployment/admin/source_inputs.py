"""List and fingerprint explicit server overlay inputs; never invoke a compiler."""
import argparse
import hashlib
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
JAVA = REPO / "server/source/src/com/oldmountain/dzm"


def sources():
    return sorted((JAVA / "admin").glob("*.java")) + [JAVA / name for name in (
        "net/packet/PacketHandler.java", "packethandler/LoginHandler.java", "packethandler/DisposeSysMsgInfoHandler.java",
        "packethandler/AdventureContinueLoginHandler.java", "packethandler/UseToolHandler.java",
        "services/RewardServices.java", "util/BattleUtil.java", "util/FateRequirement.java",
        "packethandler/AdventureInfoHandler.java", "persist/DataLander.java", "net/coderc/Encoder.java")]


def fingerprint():
    files = sources() + [Path(__file__).resolve(), REPO / "deployment/Build-Admin.ps1",
                         REPO / "deployment/admin/export_names.py", REPO / "server/distribution/DZM.jar"]
    files += [REPO / "resources/client" / name for name in (
        "Disciples.txt", "Equips.txt", "Skills.txt", "Tools.txt", "SkillBooks.txt")]
    value = hashlib.sha256()
    for path in sorted(files):
        relative = path.relative_to(REPO).as_posix().encode("utf-8")
        value.update(relative + b"\0" + hashlib.sha256(path.read_bytes()).digest())
    return value.hexdigest()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sources", action="store_true")
    args = parser.parse_args()
    if args.sources:
        for source in sources():
            print(source)
    else:
        print(fingerprint())
