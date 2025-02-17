set version 5.7.5
set pyreq ">=3.7"

file copy -force ../sdds/sdds.py src/sdds/sdds.py

if {$tcl_platform(os) == "Linux"} {
    set binDir /home/oxygen/SOLIDAY/miniconda3/bin
    file copy -force ../lib/Linux-x86_64/libsddsdata.so src/sdds/sddsdata.so
    set files {"sddsdata.so"}
} elseif {($tcl_platform(os) == "Darwin") && ($tcl_platform(machine) == "x86_64")} {
    set binDir /Users/soliday/miniconda3/bin
    file copy -force ../lib/Darwin-x86_64/libsddsdata.so src/sdds/sddsdata.so
    set files {"sddsdata.so"}
} elseif {($tcl_platform(os) == "Darwin") && ($tcl_platform(machine) == "arm64")} {
    set pyreq ">=3.8"
    set binDir /Users/soliday/miniconda3/bin
    file copy -force ../lib/Darwin-arm64/libsddsdata.so src/sdds/sddsdata.so
    set files {"sddsdata.so"}
} elseif {$tcl_platform(os) == "Windows NT"} {
    set binDir c:\\Users\\solid\\miniconda3\\Scripts
    file copy -force ../lib/Windows-x64/sddsdata.dll src/sdds/sddsdata.pyd
    file copy -force ../../SDDS/bin/windows-x64/SDDS1.dll src/sdds/SDDS1.dll
    file copy -force ../../SDDS/bin/windows-x64/rpnlib.dll src/sdds/rpnlib.dll
    file copy -force ../../SDDS/bin/windows-x64/mdbmth.dll src/sdds/mdbmth.dll
    file copy -force ../../SDDS/bin/windows-x64/mdblib.dll src/sdds/mdblib.dll
    file copy -force ../../SDDS/bin/windows-x64/lzma.dll src/sdds/lzma.dll
    set files {"sddsdata.pyd","SDDS1.dll","rpnlib.dll","mdbmth.dll","mdblib.dll","lzma.dll"}
}

catch {exec ${binDir}/conda build purge}
catch {exec ${binDir}/conda build purge-all}


puts "Building SDDS Conda Package for Python"

file delete -force src/setup.py src/meta.yaml
if {($tcl_platform(os) == "Linux")} {
    exec ../../SDDS/bin/Linux-x86_64/replaceText src/setup.py.template src/setup.py \
	-orig=<VERSION>,<PYFILES> -repl=${version},${files}
    exec ../../SDDS/bin/Linux-x86_64/replaceText src/meta.yaml.template src/meta.yaml \
	-orig=<VERSION>,<LIBGCC> "-repl=${version},- libgcc-ng"
    exec ../../SDDS/bin/Linux-x86_64/replaceText src/conda_build_config.yaml.template \
	src/conda_build_config.yaml \
	-orig=<VER37> "-repl=- 3.7"
} elseif {($tcl_platform(os) == "Darwin") && ($tcl_platform(machine) == "x86_64")} {
    exec ../../SDDS/bin/Darwin-x86_64/replaceText src/setup.py.template src/setup.py \
	-orig=<VERSION>,<PYFILES> -repl=${version},${files}
    exec ../../SDDS/bin/Darwin-x86_64/replaceText src/meta.yaml.template src/meta.yaml \
	-orig=<VERSION>,<LIBGCC> "-repl=${version},"
    exec ../../SDDS/bin/Darwin-x86_64/replaceText src/conda_build_config.yaml.template \
	src/conda_build_config.yaml \
	-orig=<VER37> "-repl=- 3.7"
} elseif {($tcl_platform(os) == "Darwin") && ($tcl_platform(machine) == "arm64")} {
    exec ../../SDDS/bin/Darwin-arm64/replaceText src/setup.py.template src/setup.py \
	-orig=<VERSION>,<PYFILES> -repl=${version},${files}
    exec ../../SDDS/bin/Darwin-arm64/replaceText src/meta.yaml.template src/meta.yaml \
	-orig=<VERSION>,<LIBGCC> "-repl=${version},"
    exec ../../SDDS/bin/Darwin-arm64/replaceText src/conda_build_config.yaml.template \
	src/conda_build_config.yaml \
	-orig=<VER37> "-repl="
} elseif {($tcl_platform(os) == "Windows NT")} {
    exec ../../SDDS/bin/Windows-x64/replaceText src/setup.py.template src/setup.py \
	-orig=<VERSION>,<PYFILES> -repl=${version},${files}
    exec ../../SDDS/bin/Windows-x64/replaceText src/meta.yaml.template src/meta.yaml \
	-orig=<VERSION>,<LIBGCC> "-repl=${version},"
    exec ../../SDDS/bin/Windows-x64/replaceText src/conda_build_config.yaml.template \
	src/conda_build_config.yaml \
	-orig=<VER37> "-repl=- 3.7"
}
puts "\nManually run:"
puts "${binDir}/conda-build . --package-format=.conda ; rm -f src/conda_build_config.yaml  src/meta.yaml src/setup.py src/sdds/sdds.py [glob -nocomplain src/sdds/*.dll src/sdds/*.pyd src/sdds/*.so]"

