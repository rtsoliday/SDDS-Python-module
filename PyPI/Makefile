SDDS_REPO = $(wildcard ../../SDDS)
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

all:
	./build.py

clean:
	rm -rf src/dist

.PHONY: all clean
