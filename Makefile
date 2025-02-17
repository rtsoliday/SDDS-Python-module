SDDS_REPO = $(wildcard ../SDDS)
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

include Makefile.rules

DIRS = ../SDDS/zlib
DIRS += ../SDDS/lzma
DIRS += ../SDDS/mdblib
DIRS += ../SDDS/mdbmth
DIRS += ../SDDS/rpns/code
DIRS += ../SDDS/SDDSlib
DIRS += ../SDDS/fftpack
DIRS += ../SDDS/matlib
DIRS += ../SDDS/mdbcommon
DIRS += ../SDDS/utils
DIRS += sdds
DIRS += PyPI

.PHONY: all $(DIRS) clean distclean

all: $(DIRS)

../SDDS/zlib:
	$(MAKE) -C $@
../SDDS/lzma: ../SDDS/zlib
	$(MAKE) -C $@
../SDDS/mdblib: ../SDDS/lzma
	$(MAKE) -C $@
../SDDS/mdbmth: ../SDDS/mdblib
	$(MAKE) -C $@
../SDDS/rpns/code: ../SDDS/mdbmth
	$(MAKE) -C $@
../SDDS/SDDSlib: ../SDDS/rpns/code
	$(MAKE) -C $@
../SDDS/fftpack: ../SDDS/SDDSlib
	$(MAKE) -C $@
../SDDS/matlib: ../SDDS/fftpack
	$(MAKE) -C $@
../SDDS/mdbcommon: ../SDDS/matlib
	$(MAKE) -C $@
../SDDS/utils: ../SDDS/mdbcommon
	$(MAKE) -C $@
sdds: ../SDDS/utils
	$(MAKE) -C $@
PyPI: sdds
	$(MAKE) -C $@

clean:
	$(MAKE) -C sdds clean
	$(MAKE) -C PyPI clean

distclean: clean
	rm -rf bin/$(OS)-$(ARCH)
	rm -rf lib/$(OS)-$(ARCH)
