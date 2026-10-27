.DEFAULT_GOAL := all
SDDS_REPO = $(firstword $(wildcard ../../SDDS ../../../../../epics/extensions/src/SDDS))
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

ENABLE_OPENMP ?= 1
BUNDLE_OPENMP ?= 1
include ../Makefile.rules
all:
	$(PYTHON) build.py

clean:
	rm -rf src/dist

.PHONY: all clean
