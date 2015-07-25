
function monster_project(_name, _kind, _defines)

	project ("monster" .. _name)
		kind (_kind)

		includedirs {
			MONSTER_DIR .. "_engine",
			MONSTER_DIR .. "_engine/core",
			MONSTER_DIR .. "_engine/common",
			MONSTER_DIR .. "_engine/render",
			MONSTER_DIR .. "_engine/physics",
			MONSTER_DIR .. "_engine/animation",
			MONSTER_THIRD_DIR .. "_3rdparty/bgfx/include",
			MONSTER_THIRD_DIR .. "_3rdparty/bx/include",
		}

		defines {
			_defines,
		}

		links {
			"bgfx"
		}

		configuration { "debug or development" }
			flags {
				"Symbols"
			}
			defines {
				"_DEBUG",
				"MONSTER_DEBUG=1"
			}

		configuration { "release" }
			defines {
				"NDEBUG"
			}

		configuration { "vs*" }
			includedirs {
				"$(DXSDK_DIR)/Include",
			}

		configuration { "vs*" }
			links {
				"OpenGL32",
				"dbghelp",
			}

		configuration { "debug", "x32", "vs*"}
			links {
			}

		configuration { "debug", "x64", "vs*" }
			links {
			}

		configuration { "development", "x32", "vs*" }
			links {
			}

		configuration { "development", "x64", "vs*" }
			links {
			}

		configuration { "release", "x32", "vs*" }
			links {
			}

		configuration { "release", "x64", "vs*" }
			links {
			}

		configuration {}

		files {
			MONSTER_DIR .. "_engine/**.h",
			MONSTER_DIR .. "_engine/**.cpp",
			MONSTER_DIR .. "_engine/*/**.h",
			MONSTER_DIR .. "_engine/*/**.cpp"
		}

		strip()

		configuration {} -- reset configuration
end
