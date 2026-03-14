import argparse
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DEFAULT_GPP = r"C:\msys64\ucrt64\bin\g++.exe"


def find_gpp(explicit_path: str | None) -> str:
    if explicit_path:
        return explicit_path
    if os.environ.get("GPP"):
        return os.environ["GPP"]
    in_path = shutil.which("g++")
    if in_path:
        return in_path
    return DEFAULT_GPP


def build(gpp: str, output: Path) -> None:
    sources = [
        ROOT / "System_Checker.cpp",
        ROOT / "src" / "SystemCheckerApp.cpp",
        ROOT / "src" / "ConsoleRenderer.cpp",
        ROOT / "src" / "CpuMonitor.cpp",
        ROOT / "src" / "RamMonitor.cpp",
        ROOT / "src" / "DiskMonitor.cpp",
        ROOT / "src" / "NetworkMonitor.cpp",
        ROOT / "src" / "WsaSession.cpp",
    ]

    missing = [str(p) for p in sources if not p.exists()]
    if missing:
        raise FileNotFoundError(
            "Missing source files:\n" + "\n".join(missing)
        )

    cmd = [
        gpp,
        "-O2",
        "-I",
        str(ROOT / "include"),
        *map(str, sources),
        "-lws2_32",
        "-liphlpapi",
        "-o",
        str(output),
    ]

    print("Building:", " ".join(cmd))
    subprocess.run(cmd, check=True, cwd=str(ROOT))


def run(exe: Path) -> None:
    if not exe.exists():
        raise FileNotFoundError(f"Output not found: {exe}")
    subprocess.run([str(exe)], check=True, cwd=str(ROOT))


def main() -> int:
    parser = argparse.ArgumentParser(description="Build and run System Checker.")
    parser.add_argument("--gpp", help="Path to g++.exe (or set GPP env var).")
    parser.add_argument("--output", default="System_Checker.exe", help="Output exe name.")
    parser.add_argument("--build-only", action="store_true", help="Only build, do not run.")
    parser.add_argument("--run-only", action="store_true", help="Only run, do not build.")
    parser.add_argument("--clean", action="store_true", help="Delete output exe and exit.")
    args = parser.parse_args()

    output = ROOT / args.output
    if args.clean:
        if output.exists():
            output.unlink()
            print(f"Deleted {output}")
        return 0

    gpp = find_gpp(args.gpp)
    if not Path(gpp).exists() and not shutil.which(gpp):
        print(f"g++ not found: {gpp}", file=sys.stderr)
        return 2

    if not args.run_only:
        build(gpp, output)
    if not args.build_only:
        run(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
