#
# Copyright 2014-2015 hyv1001. All rights reserved.
# License: http://www.opensource.org/licenses/BSD-2-Clause
#

UNAME := $(shell uname)
ifeq ($(UNAME),$(filter $(UNAME),Linux Darwin FreeBSD GNU/kFreeBSD))
ifeq ($(UNAME),$(filter $(UNAME),Darwin))
OS=darwin
else
ifeq ($(UNAME),$(filter $(UNAME),FreeBSD GNU/kFreeBSD))
OS=bsd
else
OS=linux
endif
endif
else
OS=windows
endif

# $(info $(OS))

GENIE= _3rdparty/bx/tools/bin/$(OS)/genie

all:
	$(GENIE) vs2010
	$(GENIE) vs2012
	$(GENIE) vs2013
	$(GENIE) vs2015
	$(GENIE) gmake
	$(GENIE) --gcc=linux-gcc gmake

.build/projects/gmake-linux:
	$(GENIE) --gcc=linux-gcc gmake
linux-debug32: .build/projects/gmake-linux
	$(MAKE) -R -C .build/projects/gmake-linux config=debug32
linux-release32: .build/projects/gmake-linux
	$(MAKE) -R -C .build/projects/gmake-linux config=release32
linux-debug64: .build/projects/gmake-linux
	$(MAKE) -R -C .build/projects/gmake-linux config=debug64
linux-release64: .build/projects/gmake-linux
	$(MAKE) -R -C .build/projects/gmake-linux config=release64
linux: linux-debug32 linux-release32 linux-debug64 linux-release64


.build/projects/vs2010:
	$(GENIE) vs2010

.build/projects/vs2012:
	$(GENIE) vs2012

.build/projects/vs2013:
	$(GENIE) vs2013

.build/projects/vs2015:
	$(GENIE) vs2015

rebuild-shaders:
	$(MAKE) -R -C examples rebuild

analyze:
	cppcheck src/
	cppcheck examples/

docs:
	doxygen scripts/bgfx.doxygen
	markdown README.md > .build/docs/readme.html

clean:
	@echo Cleaning...
	-@rm -rf .build

###

SILENT ?= @

UNAME := $(shell uname)
ifeq ($(UNAME),$(filter $(UNAME),Linux Darwin FreeBSD GNU/kFreeBSD))
ifeq ($(UNAME),$(filter $(UNAME),Darwin))
OS=darwin
BUILD_PROJECT_DIR=gmake-osx
BUILD_OUTPUT_DIR=osx64_clang
BUILD_TOOLS_CONFIG=release64
BUILD_TOOLS_SUFFIX=Release
EXE=
else
ifeq ($(UNAME),$(filter $(UNAME),FreeBSD GNU/kFreeBSD))
OS=bsd
BUILD_PROJECT_DIR=gmake-freebsd
BUILD_OUTPUT_DIR=freebsd64_gcc
BUILD_TOOLS_CONFIG=release64
BUILD_TOOLS_SUFFIX=Release
EXE=
else
OS=linux
BUILD_PROJECT_DIR=gmake-linux
BUILD_OUTPUT_DIR=linux64_gcc
BUILD_TOOLS_CONFIG=release64
BUILD_TOOLS_SUFFIX=Release
EXE=
endif
endif
else
OS=windows
BUILD_PROJECT_DIR=gmake-mingw-gcc
BUILD_OUTPUT_DIR=win32_mingw-gcc
BUILD_TOOLS_CONFIG=release32
BUILD_TOOLS_SUFFIX=Release
EXE=.exe
endif

tools/bin/$(OS)/shaderc$(EXE): .build/projects/$(BUILD_PROJECT_DIR)
	$(SILENT) $(MAKE) -C .build/projects/$(BUILD_PROJECT_DIR) -f shaderc.make config=$(BUILD_TOOLS_CONFIG)
	$(SILENT) cp .build/$(BUILD_OUTPUT_DIR)/bin/shaderc$(BUILD_TOOLS_SUFFIX)$(EXE) $(@)

tools/bin/$(OS)/geometryc$(EXE): .build/projects/$(BUILD_PROJECT_DIR)
	$(SILENT) $(MAKE) -C .build/projects/$(BUILD_PROJECT_DIR) -f geometryc.make config=$(BUILD_TOOLS_CONFIG)
	$(SILENT) cp .build/$(BUILD_OUTPUT_DIR)/bin/geometryc$(BUILD_TOOLS_SUFFIX)$(EXE) $(@)

tools: tools/bin/$(OS)/shaderc$(EXE) tools/bin/$(OS)/geometryc$(EXE)
