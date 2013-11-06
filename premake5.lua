solution 'V8'
	configurations { 'debug', 'release' }
	includedirs {'./include'}
	targetdir 'build/'
	flags {'ExtraWarnings', 'FatalWarnings'}
	buildoptions {'-std=gnu99'}
	linkoptions {'-pthread'}

project 'V8'
	language 'C'
	kind 'SharedLib'
	files {'src/*.c', 'include/v8/*.h'}
	links {'lua'}

	configuration 'debug'
		defines {'V8_DEBUG'}
		flags {'Symbols'}

	configuration 'release'
		flags {'Optimize'}


project 'example'
	language 'C'
	kind 'ConsoleApp'
	files {'example/*.c', 'example/*.h'}
	links {'V8'}

	configuration 'debug'
		defines {'V8_DEBUG'}
		flags {'Symbols'}

	configuration 'release'
		flags {'Optimize'}

include 'test'
