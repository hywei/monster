

MONSTER_DIR = (path.getabsolute("..") .. "/")
MONSTER_THIRD_DIR = (MONSTER_DIR .. "_3rdparty/")
local MONSTER_BUILD_DIR = (MONSTER_DIR .. ".build/")
BGFX_DIR = (MONSTER_DIR .. "_3rdparty/bgfx/")
BX_DIR = (MONSTER_DIR .. "_3rdparty/bx/")

function copyLib()
end


solution "monster"
	configurations {
		"debug",
		"development",
		"release",
	}

	platforms {
		"x32",
		"x64",
		"native"
	}

	language "C++"

	configuration {}

dofile ("toolchain.lua")
dofile (BGFX_DIR .. "scripts/bgfx.lua")
dofile ("monster.lua")

toolchain(MONSTER_BUILD_DIR, MONSTER_THIRD_DIR)

group "libs"
bgfxProject("", "StaticLib", os.is("windows") and { "BGFX_CONFIG_RENDERER_DIRECT3D9=1" } or {})

group "engine"
monster_project("", "ConsoleApp", {})


-- Install
configuration { "x32", "vs*" }
	postbuildcommands {
		"cp -r " .. MONSTER_DIR .. ".build/win32/bin/* " .. "$(MONSTER_INSTALL_DIR)/" .. "bin/win32/",
	}

configuration { "x64", "vs*" }
	postbuildcommands {
		"cp -r " .. MONSTER_DIR .. ".build/win64/bin/* " .. "$(MONSTER_INSTALL_DIR)/" .. "bin/win64/",
	}
