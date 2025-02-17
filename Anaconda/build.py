#!/usr/bin/env python3
import os
import platform
import shutil
import subprocess
import glob

version = "5.7.5"
pyreq = ">=3.7"

def copy_force(src, dst):
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    if os.path.exists(dst):
        os.remove(dst)
    shutil.copy2(src, dst)

# Copy main Python file
copy_force("../sdds/sdds.py", "src/sdds/sdds.py")

# Determine platform and copy the appropriate files
system = platform.system()
machine = platform.machine()
if system == "Linux":
    binDir = "/home/oxygen/SOLIDAY/miniconda3/bin"
    copy_force("../lib/Linux-x86_64/libsddsdata.so", "src/sdds/sddsdata.so")
    files_str = "sddsdata.so"
elif system == "Darwin" and machine == "x86_64":
    binDir = "/Users/soliday/miniconda3/bin"
    copy_force("../lib/Darwin-x86_64/libsddsdata.so", "src/sdds/sddsdata.so")
    files_str = "sddsdata.so"
elif system == "Darwin" and machine == "arm64":
    pyreq = ">=3.8"
    binDir = "/Users/soliday/miniconda3/bin"
    copy_force("../lib/Darwin-arm64/libsddsdata.so", "src/sdds/sddsdata.so")
    files_str = "sddsdata.so"
else:
    pyreq = ">=3.8"
    binDir = r"c:/Users/solid/miniconda3/Scripts"
    # Windows files
    copy_force("../bin/Windows-x86_64/sddsdata8.dll", "src/sdds/sddsdata8.pyd")
    copy_force("../bin/Windows-x86_64/sddsdata9.dll", "src/sdds/sddsdata9.pyd")
    copy_force("../bin/Windows-x86_64/sddsdata10.dll", "src/sdds/sddsdata10.pyd")
    copy_force("../bin/Windows-x86_64/sddsdata11.dll", "src/sdds/sddsdata11.pyd")
    copy_force("../bin/Windows-x86_64/sddsdata12.dll", "src/sdds/sddsdata12.pyd")
    copy_force("../bin/Windows-x86_64/sddsdata13.dll", "src/sdds/sddsdata13.pyd")
    copy_force("../../SDDS/bin/Windows-x86_64/SDDS1.dll", "src/sdds/SDDS1.dll")
    copy_force("../../SDDS/bin/Windows-x86_64/rpnlib.dll", "src/sdds/rpnlib.dll")
    copy_force("../../SDDS/bin/Windows-x86_64/mdbmth.dll", "src/sdds/mdbmth.dll")
    copy_force("../../SDDS/bin/Windows-x86_64/mdblib.dll", "src/sdds/mdblib.dll")
    copy_force("../../SDDS/bin/Windows-x86_64/lzma.dll", "src/sdds/lzma.dll")
    files_list = ["sddsdata.pyd", "SDDS1.dll", "rpnlib.dll", "mdbmth.dll", "mdblib.dll", "lzma.dll"]
    files_str = ",".join(files_list)

# Delete old build files
for f in ["src/setup.py", "src/meta.yaml", "src/conda_build_config.yaml"]:
    try:
        os.remove(f)
    except FileNotFoundError:
        pass

def run_cmd(cmd_list):
    subprocess.run(cmd_list, check=True)

def get_glob_files():
    matches = glob.glob("src/sdds/*.dll") + glob.glob("src/sdds/*.pyd") + glob.glob("src/sdds/*.so")
    return " ".join(matches)

# Execute replacement commands based on platform
if system == "Linux":
    run_cmd(["../../SDDS/bin/Linux-x86_64/replaceText",
             "src/setup.py.template", "src/setup.py",
             "-orig=<VERSION>,<PYFILES>", f"-repl={version},{files_str}"])
    run_cmd(["../../SDDS/bin/Linux-x86_64/replaceText",
             "src/meta.yaml.template", "src/meta.yaml",
             "-orig=<VERSION>,<LIBGCC>", f"-repl={version},- libgcc-ng"])
    run_cmd(["../../SDDS/bin/Linux-x86_64/replaceText",
             "src/conda_build_config.yaml.template", "src/conda_build_config.yaml",
             "-orig=<VER37>", "-repl=- 3.7"])
    print("\nManually run from Anaconda directory:")
    print(f"{binDir}/conda-build . --package-format=.conda ; rm -f src/conda_build_config.yaml src/meta.yaml src/setup.py src/sdds/sdds.py {get_glob_files()}")
elif system == "Darwin" and machine == "x86_64":
    run_cmd(["../../SDDS/bin/Darwin-x86_64/replaceText",
             "src/setup.py.template", "src/setup.py",
             "-orig=<VERSION>,<PYFILES>", f"-repl={version},{files_str}"])
    run_cmd(["../../SDDS/bin/Darwin-x86_64/replaceText",
             "src/meta.yaml.template", "src/meta.yaml",
             "-orig=<VERSION>,<LIBGCC>", f"-repl={version},"])
    run_cmd(["../../SDDS/bin/Darwin-x86_64/replaceText",
             "src/conda_build_config.yaml.template", "src/conda_build_config.yaml",
             "-orig=<VER37>", "-repl=- 3.7"])
    print("\nManually run from Anaconda directory:")
    print(f"{binDir}/conda-build . --package-format=.conda ; rm -f src/conda_build_config.yaml src/meta.yaml src/setup.py src/sdds/sdds.py {get_glob_files()}")
elif system == "Darwin" and machine == "arm64":
    run_cmd(["../../SDDS/bin/Darwin-arm64/replaceText",
             "src/setup.py.template", "src/setup.py",
             "-orig=<VERSION>,<PYFILES>", f"-repl={version},{files_str}"])
    run_cmd(["../../SDDS/bin/Darwin-arm64/replaceText",
             "src/meta.yaml.template", "src/meta.yaml",
             "-orig=<VERSION>,<LIBGCC>", f"-repl={version},"])
    run_cmd(["../../SDDS/bin/Darwin-arm64/replaceText",
             "src/conda_build_config.yaml.template", "src/conda_build_config.yaml",
             "-orig=<VER37>", "-repl="])
    print("\nManually run from Anaconda directory:")
    print(f"{binDir}/conda-build . --package-format=.conda ; rm -f src/conda_build_config.yaml src/meta.yaml src/setup.py src/sdds/sdds.py {get_glob_files()}")
else:
    run_cmd(["../../SDDS/bin/Windows-x86_64/replaceText",
             "src/setup.py.template", "src/setup.py",
             "-orig=<VERSION>,<PYFILES>", f"-repl={version},({files_str})"])
    run_cmd(["../../SDDS/bin/Windows-x86_64/replaceText",
             "src/meta.yaml.template", "src/meta.yaml",
             "-orig=<VERSION>,<LIBGCC>", f"-repl={version},"])
    run_cmd(["../../SDDS/bin/Windows-x86_64/replaceText",
             "src/conda_build_config.yaml.template", "src/conda_build_config.yaml",
             "-orig=<VER37>", "-repl="])
    output = (
        f"copy /Y src\\sdds\\sddsdata13.pyd src\\sdds\\sddsdata.pyd & {binDir}/conda-build . --package-format=.conda --python=3.13 & "
        f"copy /Y src\\sdds\\sddsdata12.pyd src\\sdds\\sddsdata.pyd & {binDir}/conda-build . --package-format=.conda --python=3.12 & "
        f"copy /Y src\\sdds\\sddsdata11.pyd src\\sdds\\sddsdata.pyd & {binDir}/conda-build . --package-format=.conda --python=3.11 & "
        f"copy /Y src\\sdds\\sddsdata10.pyd src\\sdds\\sddsdata.pyd & {binDir}/conda-build . --package-format=.conda --python=3.10 & "
        f"copy /Y src\\sdds\\sddsdata9.pyd src\\sdds\\sddsdata.pyd & {binDir}/conda-build . --package-format=.conda --python=3.9 & "
        f"copy /Y src\\sdds\\sddsdata8.pyd src\\sdds\\sddsdata.pyd & {binDir}/conda-build . --package-format=.conda --python=3.8"
    )
    print("\nManually run in the new window:")
    print(output)
    print("\nThen run in the cygwin window:")
    print(f"rm -f src/conda_build_config.yaml src/meta.yaml src/setup.py src/sdds/sdds.py {get_glob_files()}")
    run_cmd(["cygstart", "cmd.exe", "/K",
             r"C:\Users\solid\miniconda3\Scripts\activate.bat",
             r"C:\Users\solid\miniconda3"])
