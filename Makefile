# Detect OS and Architecture
OS := $(shell uname -s)
ifeq ($(findstring CYGWIN, $(OS)),CYGWIN)
    OS := Windows
endif

# Check for external gsl repository needed on Windows
ifeq ($(OS), Windows)
  GSL_REPO = $(wildcard ../gsl)
  ifeq ($(GSL_REPO),)
    $(error GSL source code not found. Run 'git clone https://github.com/rtsoliday/gsl.git' next to the SDDS-Python-module repository)
  endif
endif

# Check for external SDDS repository
SDDS_REPO = $(firstword $(wildcard ../SDDS ../../../../epics/extensions/src/SDDS))
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

include Makefile.rules

DIRS = $(GSL_REPO)
DIRS += $(SDDS_REPO)/zlib
DIRS += $(SDDS_REPO)/lzma
DIRS += $(SDDS_REPO)/mdblib
DIRS += $(SDDS_REPO)/mdbmth
DIRS += $(SDDS_REPO)/rpns/code
DIRS += $(SDDS_REPO)/SDDSlib
DIRS += $(SDDS_REPO)/fftpack
DIRS += $(SDDS_REPO)/matlib
DIRS += $(SDDS_REPO)/mdbcommon
DIRS += $(SDDS_REPO)/utils
DIRS += sdds
DIRS += PyPI
DIRS += Anaconda

.PHONY: all $(DIRS) clean distclean

all: $(DIRS)

ifneq ($(GSL_REPO),)
  $(GSL_REPO):
	$(MAKE) -C $@ -f Makefile.MSVC all
endif
$(SDDS_REPO)/zlib:
	$(MAKE) -C $@
$(SDDS_REPO)/lzma: $(SDDS_REPO)/zlib
	$(MAKE) -C $@
$(SDDS_REPO)/mdblib: $(SDDS_REPO)/lzma
	$(MAKE) -C $@
$(SDDS_REPO)/mdbmth: $(SDDS_REPO)/mdblib
	$(MAKE) -C $@
$(SDDS_REPO)/rpns/code: $(SDDS_REPO)/mdbmth $(GSL_REPO)
	$(MAKE) -C $@
$(SDDS_REPO)/SDDSlib: $(SDDS_REPO)/rpns/code
	$(MAKE) -C $@
$(SDDS_REPO)/fftpack: $(SDDS_REPO)/SDDSlib
	$(MAKE) -C $@
$(SDDS_REPO)/matlib: $(SDDS_REPO)/fftpack
	$(MAKE) -C $@
$(SDDS_REPO)/mdbcommon: $(SDDS_REPO)/matlib
	$(MAKE) -C $@
$(SDDS_REPO)/utils: $(SDDS_REPO)/mdbcommon
	$(MAKE) -C $@
ifeq ($(OS), Windows)
sdds: $(SDDS_REPO)/utils
	$(MAKE) -C $@ clean
	$(MAKE) PY=8 -C $@
	$(MAKE) -C $@ clean
	$(MAKE) PY=9 -C $@
	$(MAKE) -C $@ clean
	$(MAKE) PY=10 -C $@
	$(MAKE) -C $@ clean
	$(MAKE) PY=11 -C $@
	$(MAKE) -C $@ clean
	$(MAKE) PY=12 -C $@
	$(MAKE) -C $@ clean
	$(MAKE) PY=13 -C $@
else
sdds: $(SDDS_REPO)/utils
	$(MAKE) -C $@
endif
PyPI: sdds
	$(MAKE) -C $@
Anaconda: PyPI
	$(MAKE) -C $@

clean:
	$(MAKE) -C sdds clean
	$(MAKE) -C PyPI clean
	$(MAKE) -C Anaconda clean

distclean: clean
	rm -rf bin/$(OS)-$(ARCH)
	rm -rf lib/$(OS)-$(ARCH)
