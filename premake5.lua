solution 'V8'
	configurations { 'debug', 'release' }
	includedirs {'./include', './vendor/dist/include'}
	libdirs {'./vendor/dist/lib'}
	targetdir 'build/'
	flags {'FatalWarnings'}
	buildoptions {'-std=gnu99'}
	linkoptions {'-pthread'}

-- project 'lua'
-- 	language 'C'
-- 	kind 'Makefile'

-- 	buildcommands {
-- 		'make -C %{prj.location}/vendor/lua-5.2.0 linux local'
-- 	}

-- 	cleancommands {
-- 		'make -C vendor/lua-5.2.0 clean'
-- 	}

project 'V8'
	language 'C'
	kind 'SharedLib'
	warnings 'Extra'
	files { 'src/*.c', 'include/v8/*.h' }

	configuration 'debug'
		defines {'V8_DEBUG'}
		flags {'Symbols'}

	configuration 'release'
		optimize 'On'


project 'example'
	language 'C'
	kind 'ConsoleApp'
	files {'example/*.c', 'example/*.h'}
	links {'V8', 'lua', 'm'}

	configuration 'debug'
		defines {'V8_DEBUG'}
		flags {'Symbols'}

	configuration 'release'
		optimize 'On'

include 'test'
