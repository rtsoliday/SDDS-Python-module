SDDS_REPO = $(wildcard ../../SDDS)
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

include ../Makefile.rules

PYTHON_SHARED_LIBRARY = sddsdata

LIBRARY_SRC = sddsdatamodule.c

CFLAGS += $(PYTHON_INCLUDE_DIRS) -I$(SDDS_REPO)/include

ifeq ($(OS), Linux)
  LDFLAGS := -L$(SDDS_REPO)/lib/$(OS)-$(ARCH) $(LDFLAGS)
  LIBRARY_SYS_LIBS := $(LZMA_LIB) $(GSL_LIB) $(GSLCBLAS_LIB) $(Z_LIB) $(PROD_SYS_LIBS)
  LIBRARY_LIBS = -lSDDS1 -lrpnlib -lmdbmth -lmdblib
endif

ifeq ($(OS), Darwin)
  LDFLAGS := -undefined dynamic_lookup -L$(SDDS_REPO)/lib/$(OS)-$(ARCH) $(LDFLAGS)
  LIBRARY_SYS_LIBS := $(LZMA_LIB) $(GSL_LIB) $(GSLCBLAS_LIB) $(Z_LIB) $(PROD_SYS_LIBS)
  LIBRARY_LIBS = -lSDDS1 -lrpnlib -lmdbmth -lmdblib
endif

ifeq ($(OS), Windows)
  #CFLAGS += 
  LIBRARY_LIBS = SDDS1.lib rpnlib.lib mdbmth.lib mdblib.lib lzma.lib z.lib
endif

include ../Makefile.build

