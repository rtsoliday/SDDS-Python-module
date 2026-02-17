#!/usr/bin/env python3
import os
import sys
import shutil
import subprocess
import platform
import glob
import re

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

def run_optional_command(cmd):
    subprocess.run(cmd, check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

def find_first_existing(paths):
    for path in paths:
        if os.path.exists(path):
            return path
    return None

def find_first_existing_in_dirs(file_names, directories):
    for directory in directories:
        if not directory:
            continue
        for file_name in file_names:
            candidate = os.path.join(directory, file_name)
            if os.path.exists(candidate):
                return candidate
    return None

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
    version = "5.7.6"
    # Copy sdds.py
    shutil.copy(os.path.join("..", "sdds", "sdds.py"),
                os.path.join("src", "sdds", "sdds.py"))
    shutil.copy(os.path.join("..", "LICENSE"),
                os.path.join("src", "LICENSE"))
    shutil.copy(os.path.join("..", "README.md"),
                os.path.join("src", "README.md"))

    current_os = sys.platform
    machine = platform.machine()


    # Platform-specific settings
    if current_os.startswith("linux"):
        preferred_python_cmd = "/home/oxygen/SOLIDAY/miniconda3/bin/python"
        python_cmd = preferred_python_cmd if os.path.exists(preferred_python_cmd) else sys.executable
        binDir = os.path.dirname(python_cmd)
        prefix_dir = os.path.dirname(binDir)
        conda_prefix = os.environ.get("CONDA_PREFIX")
        lib_search_dirs = [
            os.path.join(prefix_dir, "lib"),
            os.path.join(prefix_dir, "lib64"),
            os.path.join("/usr/local/oag/apps", "lib", "linux-x86_64"),
            os.path.join("/usr/local/oag/apps", "lib"),
            "/usr/lib64",
            "/usr/lib/x86_64-linux-gnu",
            "/usr/lib",
        ]
        if conda_prefix:
            lib_search_dirs = [
                os.path.join(conda_prefix, "lib"),
                os.path.join(conda_prefix, "lib64"),
                *lib_search_dirs,
            ]
        tag = "manylinux_2_17_x86_64"
        shutil.copy(os.path.join("..", "lib", "Linux-x86_64", "libsddsdata.so"),
                    os.path.join("src", "sdds", "sddsdata.so"))

        gsl_path = find_first_existing_in_dirs(
            ["libgsl.so.27", "libgsl.so"],
            lib_search_dirs,
        )
        gslcblas_path = find_first_existing_in_dirs(
            ["libgslcblas.so.0", "libgslcblas.so"],
            lib_search_dirs,
        )

        if not gsl_path or not gslcblas_path:
            print("ERROR: Could not locate GSL libraries in any known Linux lib directory")
            print("Searched directories:", ", ".join(lib_search_dirs))
            print("Expected libgsl.so.27/libgsl.so and libgslcblas.so.0/libgslcblas.so")
            sys.exit(1)

        shutil.copy(gsl_path, os.path.join("src", "sdds", "libgsl.so.27"))
        shutil.copy(gslcblas_path, os.path.join("src", "sdds", "libgslcblas.so.0"))

        patchelf = shutil.which("patchelf")
        if not patchelf:
            print("ERROR: patchelf is required on Linux to fix shared library references")
            sys.exit(1)

        sdds_so = os.path.join("src", "sdds", "sddsdata.so")
        run_command([patchelf, "--set-rpath", "$ORIGIN", sdds_so])
        run_optional_command([patchelf, "--replace-needed", "/lib64/libgsl.so.23", "libgsl.so.27", sdds_so])
        run_optional_command([patchelf, "--replace-needed", "libgsl.so.23", "libgsl.so.27", sdds_so])
        run_optional_command([patchelf, "--replace-needed", "/lib64/libgslcblas.so.0", "libgslcblas.so.0", sdds_so])

        replace_path = os.path.join("..", "..", "SDDS", "bin", "Linux-x86_64", "replaceText")
        files_str = '("sdds.py", "sddsdata.so", "libgsl.so.27", "libgslcblas.so.0")'

    elif current_os == "darwin" and machine in ("x86_64", "arm64"):
        home_dir = os.path.expanduser("~")
        binDir = os.path.join(home_dir, "miniconda3", "bin")
        tag = "macosx_11_0_x86_64" if machine == "x86_64" else "macosx_11_0_arm64"
        python_exe = "python3"
        lib_dir = "Darwin-x86_64" if machine == "x86_64" else "Darwin-arm64"
        shutil.copy(os.path.join("..", "lib", lib_dir, "libsddsdata.so"),
                    os.path.join("src", "sdds", "sddsdata.so"))
        replace_path = os.path.join("..", "..", "SDDS", "bin", lib_dir, "replaceText")
        files_str = '("sdds.py", "sddsdata.so")'

    elif current_os.startswith("win"):
        home_dir = os.environ.get("USERPROFILE")
        if not home_dir:
            home_dir = os.path.join(os.environ.get("HOMEDRIVE", ""), os.environ.get("HOMEPATH", ""))
        binDir = os.path.join(home_dir, "miniconda3")
        #binDir = r"c:\Users\solid\miniconda3"
        tag = "win_amd64"
        python_exe = "python.exe"
        windows_bin = os.path.join("..", "bin", "Windows-x86_64")
        sdds_dlls = sorted(glob.glob(os.path.join(windows_bin, "sddsdata*.dll")))
        sdds_pyds = []
        minor_to_module = {}

        for dll_path in sdds_dlls:
            dll_name = os.path.basename(dll_path)
            match = re.match(r"sddsdata(\d+)\.dll$", dll_name)
            if not match:
                continue
            minor = int(match.group(1))
            module_name = f"sddsdata{minor}.pyd"
            shutil.copy(dll_path, os.path.join("src", "sdds", module_name))
            sdds_pyds.append(module_name)
            minor_to_module[minor] = module_name

        if not sdds_pyds:
            print("ERROR: No sddsdata*.dll files were found in", windows_bin)
            sys.exit(1)

        preferred_minor = sys.version_info.minor
        if preferred_minor in minor_to_module:
            fallback_module = minor_to_module[preferred_minor]
        else:
            fallback_module = minor_to_module[max(minor_to_module.keys())]

        shutil.copy(os.path.join("src", "sdds", fallback_module),
                    os.path.join("src", "sdds", "sddsdata.pyd"))
        dlls = ["SDDS1.dll", "rpnlib.dll", "mdbmth.dll", "mdblib.dll", "lzma.dll"]
        for dll in dlls:
            shutil.copy(os.path.join("..", "..", "SDDS", "bin", "Windows-x86_64", dll),
                        os.path.join("src", "sdds", dll))
        replace_path = os.path.join("..", "..", "SDDS", "bin", "Windows-x86_64", "replaceText")
        files_list = ["sdds.py", "sddsdata.pyd", *sdds_pyds, *dlls]
        files_str = "(" + ", ".join(f'\"{name}\"' for name in files_list) + ")"

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

    if current_os.startswith("linux"):
        build_cmd = [python_cmd, "-m", "build"]
    else:
        build_cmd = [os.path.join(binDir, python_exe), "-m", "build"]
    build_output = run_command(build_cmd)
    print(build_output)
    output_file = build_output.split()[-1]

    if current_os.startswith("linux"):
        wheel_cmd = [
            python_cmd,
            "-m", "wheel", "tags", "--platform-tag", tag,
            os.path.join("dist", output_file)
        ]
    else:
        wheel_cmd = [
            os.path.join(binDir, python_exe),
            "-m", "wheel", "tags", "--platform-tag", tag,
            os.path.join("dist", output_file)
        ]
    run_command(wheel_cmd)

    # Clean up temporary files (currently commented out)
    cleanup_files = [
        "LICENSE",
        "README.md",
        "pyproject.toml",
        os.path.join("sdds", "sdds.py"),
        os.path.join("sdds", "sddsdata.so"),
        os.path.join("sdds", "libgsl.so.27"),
        os.path.join("sdds", "libgslcblas.so.0"),
        os.path.join("sdds", "sddsdata.pyd"),
        os.path.join("sdds", "sddsdata8.pyd"),
        os.path.join("sdds", "sddsdata9.pyd"),
        os.path.join("sdds", "sddsdata10.pyd"),
        os.path.join("sdds", "sddsdata11.pyd"),
        os.path.join("sdds", "sddsdata12.pyd"),
        os.path.join("sdds", "sddsdata13.pyd"),
        os.path.join("sdds", "sddsdata14.pyd"),
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
    if current_os.startswith("linux"):
        print(f"{python_cmd} -m twine upload --verbose {pkg_path}")
    else:
        print(f"{os.path.join(binDir, python_exe)} -m twine upload --verbose {pkg_path}")

if __name__ == "__main__":
    main()
