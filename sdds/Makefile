.DEFAULT_GOAL := all
SDDS_REPO = $(firstword $(wildcard ../../SDDS ../../../../../epics/extensions/src/SDDS))
ifeq ($(SDDS_REPO),)
  $(error SDDS source code not found. Run 'git clone https://github.com/rtsoliday/SDDS.git' next to the SDDS-Python-module repository)
endif

ENABLE_OPENMP ?= 1
BUNDLE_OPENMP ?= 1
include ../Makefile.rules
PYTHON_SHARED_LIBRARY = sddsdata$(PY)

LIBRARY_SRC = sddsdatamodule.c

CFLAGS += $(PYTHON_INCLUDE_DIRS) -I$(SDDS_REPO)/include

ifeq ($(OS), Linux)
  LDFLAGS := -L$(SDDS_REPO)/lib/$(OS)-$(ARCH) $(LDFLAGS)
  LIBRARY_SYS_LIBS := $(LZMA_LIB) $(GSL_LIB) $(GSLCBLAS_LIB) $(Z_LIB) $(PROD_SYS_LIBS)
  LIBRARY_LIBS_SDDS = -lSDDS1 -lrpnlib -lmdbmth -lmdblib
endif

ifeq ($(OS), Darwin)
  LDFLAGS := -undefined dynamic_lookup -L$(SDDS_REPO)/lib/$(OS)-$(ARCH) $(LDFLAGS)
  LIBRARY_SYS_LIBS := $(LZMA_LIB) $(GSL_LIB) $(GSLCBLAS_LIB) $(Z_LIB) $(PROD_SYS_LIBS)
  LIBRARY_LIBS_SDDS = -lSDDS1 -lrpnlib -lmdbmth -lmdblib
endif

ifeq ($(OS), Windows)
  CFLAGS += -I$(SDDS_REPO)/lzma
  LIBRARY_SYS_LIBS = $(PYTHON_PREFIX)/libs/python3.lib
  LIBRARY_LIBS_SDDS = SDDS1.lib rpnlib.lib gsl.lib gslcblas.lib mdbmth.lib mdblib.lib lzma.lib z.lib 
  LIB_LINK_DIRS += -LIBPATH:$(SDDS_REPO)/lib/$(OS)-$(ARCH) -LIBPATH:$(PYTHON_PREFIX)/libs
endif

ifneq ($(OS),Windows)
LIBRARY_LIBS += $(LIBRARY_LIBS_SDDS)
endif
ifeq ($(OS),Linux)
LIBRARY_SYS_LIBS += -fopenmp
endif
include ../Makefile.build
$(BUILD_GENERATED): | $(OBJ_DIR)