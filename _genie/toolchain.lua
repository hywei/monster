
function toolchain(build_dir, lib_dir)

	if (_ACTION == nil) then return end

	location (build_dir .. "projects/" .. _ACTION)

	if _ACTION == "clean" then
		os.rmdir(BUILD_DIR)
	end

	if _ACTION == "vs2012" or _ACTION == "vs2013" then

		if not os.is("windows") then print("Action not valid in current OS.") end


		if not os.getenv("DXSDK_DIR") then
			print("Set DXSDK_DIR environment variable.")
		end

		location(build_dir .. "projects/" .. "windows")
	end

	flags {
		"StaticRuntime",
		"NoPCH",
		"NoRTTI",
		"NoExceptions",
		"NoEditAndContinue",
		"Symbols",
	}

	defines {
		"__STDC_FORMAT_MACROS",
		"__STDC_CONSTANT_MACROS",
		"__STDC_LIMIT_MACROS",
	}

	configuration { "development or release" }
		flags {
			"OptimizeSpeed"
		}

	configuration { "debug", "x32" }
		targetsuffix "-debug-32"
	configuration { "debug", "x64" }
		targetsuffix "-debug-64"

	configuration { "development", "x32" }
		targetsuffix "-development-32"
	configuration { "development", "x64" }
		targetsuffix "-development-64"

	configuration { "release", "x32" }
		targetsuffix "-release-32"
	configuration { "release", "x64" }
		targetsuffix "-release-64"

	configuration { "debug", "native" }
		targetsuffix "-debug"

	configuration { "development", "native" }
		targetsuffix "-development"

	configuration { "release", "native" }
		targetsuffix "-release"

	configuration { "vs*" }
		defines {
			"WIN32",
			"_WIN32",
			"_HAS_EXCEPTIONS=0",
			"_HAS_ITERATOR_DEBUGGING=0",
			"_SCL_SECURE=0",
			"_SECURE_SCL=0",
			"_SCL_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_SECURE_NO_DEPRECATE",
			"NOMINMAX",
		}
		buildoptions {
			"/Oy-", -- Suppresses creation of frame pointers on the call stack.
			"/Ob2", -- The Inline Function Expansion
		}
		linkoptions {
			"/ignore:4199", -- LNK4199: /DELAYLOAD:*.dll ignored; no imports found from *.dll
			"/ignore:4221", -- LNK4221: This object file does not define any previously undefined public symbols, so it will not be used by any link operation that consumes this library
			"/ignore:4099", -- LNK4099: The linker was unable to find your .pdb file.
		}

	configuration { "x32", "vs*" }
		targetdir (build_dir .. "win32" .. "/bin")
		objdir (build_dir .. "win32" .. "/obj")
		libdirs {
			"$(DXSDK_DIR)/Lib/x86",
		}

	configuration { "x64", "vs*" }
		targetdir (build_dir .. "win64" .. "/bin")
		objdir (build_dir .. "win64" .. "/obj")
		libdirs {
			"$(DXSDK_DIR)/Lib/x64",
		}

	configuration {} -- reset configuration
end

function strip()

	configuration {} -- reset configuration
end
