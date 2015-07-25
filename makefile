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

GENIE_FILE= _genie/genie.lua

all:
	$(GENIE) --file=$(GENIE_FILE) vs2013

.build/projects/vs2013:
	$(GENIE) --file=$(GENIE_FILE) vs2013

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

tools/bin/$(OS)/shaderc$(EXE): .build/projects/$(BUILD_PROJECT_DIR)
	$(SILENT) $(MAKE) -C .build/projects/$(BUILD_PROJECT_DIR) -f shaderc.make config=$(BUILD_TOOLS_CONFIG)
	$(SILENT) cp .build/$(BUILD_OUTPUT_DIR)/bin/shaderc$(BUILD_TOOLS_SUFFIX)$(EXE) $(@)

tools/bin/$(OS)/geometryc$(EXE): .build/projects/$(BUILD_PROJECT_DIR)
	$(SILENT) $(MAKE) -C .build/projects/$(BUILD_PROJECT_DIR) -f geometryc.make config=$(BUILD_TOOLS_CONFIG)
	$(SILENT) cp .build/$(BUILD_OUTPUT_DIR)/bin/geometryc$(BUILD_TOOLS_SUFFIX)$(EXE) $(@)

tools: tools/bin/$(OS)/shaderc$(EXE) tools/bin/$(OS)/geometryc$(EXE)
