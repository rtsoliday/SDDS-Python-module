SDDS_REPO = $(wildcard ../../SDDS)
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

include ../Makefile.rules

all:
	tclsh build.tcl

clean:
	rm -rf src/conda_build_config.yaml src/meta.yaml src/setup.py src/sdds/sdds.py src/sdds/*.so src/sdds/*.pyd src/sdds/*.dll

.PHONY: all clean
