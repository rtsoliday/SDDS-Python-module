#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess
import platform

def remove_path(path):
    if os.path.exists(path):
        if os.path.isdir(path):
            shutil.rmtree(path, ignore_errors=True)
        else:
            os.remove(path)

def run_command(cmd):
    try:
        result = subprocess.run(
            cmd, check=True,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            text=True
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        print("ERROR:", e.stderr)
        sys.exit(1)

def run_replace_text(replace_path, version, files_str):
    cmd = [
        replace_path,
        "src/pyproject.toml.template",
        "src/pyproject.toml",
        "-orig=<PYVERSION>,<PYREQPYVER>,<PYFILES>",
        f"-replacement={version},>=3.7,{files_str}"
    ]
    subprocess.run(cmd, check=True)

def main():
    version = "5.7.5"
    # Copy sdds.py
    shutil.copy(os.path.join("..", "sdds", "sdds.py"),
                os.path.join("src", "sdds", "sdds.py"))

    current_os = sys.platform
    machine = platform.machine()

    # Platform-specific settings
    if current_os.startswith("linux"):
        binDir = "/home/oxygen/SOLIDAY/miniconda3/bin"
        tag = "manylinux_2_17_x86_64"
        python_exe = "python3"
        shutil.copy(os.path.join("..", "lib", "Linux-x86_64", "libsddsdata.so"),
                    os.path.join("src", "sdds", "sddsdata.so"))
        replace_path = os.path.join("..", "..", "SDDS", "bin", "Linux-x86_64", "replaceText")
        files_str = '("sdds.py", "sddsdata.so")'

    elif current_os == "darwin" and machine in ("x86_64", "arm64"):
        binDir = "/Users/soliday/miniconda3/bin/"
        tag = "macosx_11_0_x86_64" if machine == "x86_64" else "macosx_11_0_arm64"
        python_exe = "python3"
        lib_dir = "Darwin-x86_64" if machine == "x86_64" else "Darwin-arm64"
        shutil.copy(os.path.join("..", "lib", lib_dir, "libsddsdata.so"),
                    os.path.join("src", "sdds", "sddsdata.so"))
        replace_path = os.path.join("..", "..", "SDDS", "bin", lib_dir, "replaceText")
        files_str = '("sdds.py", "sddsdata.so")'

    elif current_os.startswith("win"):
        binDir = r"c:\Users\solid\miniconda3"
        tag = "win_amd64"
        python_exe = "python.exe"
        shutil.copy(os.path.join("..", "bin", "Windows-x86_64", "sddsdata.dll"),
                    os.path.join("src", "sdds", "sddsdata.pyd"))
        dlls = ["SDDS1.dll", "rpnlib.dll", "mdbmth.dll", "mdblib.dll", "lzma.dll"]
        for dll in dlls:
            shutil.copy(os.path.join("..", "..", "SDDS", "bin", "Windows-x86_64", dll),
                        os.path.join("src", "sdds", dll))
        replace_path = os.path.join("..", "..", "SDDS", "bin", "Windows-x86_64", "replaceText")
        files_str = '("sdds.py", "sddsdata.pyd", "SDDS1.dll", "rpnlib.dll", "mdbmth.dll", "mdblib.dll", "lzma.dll")'

    else:
        print("Unsupported platform")
        sys.exit(1)

    # Run the replaceText command
    run_replace_text(replace_path, version, files_str)

    os.chdir("src")
    print("Building sdds PyPI Package for Python")

    # Delete build artifacts (currently commented out)
    for path in ["build", "dist", "soliday.sdds.egg-info"]:
        remove_path(path)

    build_cmd = [os.path.join(binDir, python_exe), "-m", "build"]
    build_output = run_command(build_cmd)
    print(build_output)
    output_file = build_output.split()[-1]

    wheel_cmd = [
        os.path.join(binDir, python_exe),
        "-m", "wheel", "tags", "--platform-tag", tag,
        os.path.join("dist", output_file)
    ]
    run_command(wheel_cmd)

    # Clean up temporary files (currently commented out)
    cleanup_files = [
        "pyproject.toml",
        os.path.join("sdds", "sdds.py"),
        os.path.join("sdds", "sddsdata.so"),
        os.path.join("sdds", "sddsdata.pyd"),
        os.path.join("sdds", "SDDS1.dll"),
        os.path.join("sdds", "rpnlib.dll"),
        os.path.join("sdds", "mdbmth.dll"),
        os.path.join("sdds", "mdblib.dll"),
        os.path.join("sdds", "lzma.dll"),
        "soliday.sdds.egg-info",
        os.path.join("dist", output_file)
    ]
    for f in cleanup_files:
        remove_path(f)

    pkg_path = os.path.abspath(os.path.join("dist", output_file))
    pkg_path = f"{pkg_path[:-7]}{tag}.whl"
    print(f"\nPyPI Package {pkg_path} created")
    print("Upload to PyPI.org using command:")
    print(f"{os.path.join(binDir, python_exe)} -m twine upload --verbose {pkg_path}")

if __name__ == "__main__":
    main()
