#This unit test file should be called from the parent cmakelists.txt. It's not intended to be built stand alone because it depends on the engine library and build scripts.
CMAKE_MINIMUM_REQUIRED (VERSION 3.7.0)

InitModule()

SET(entryPointRoot ${CMAKE_SOURCE_DIR}/Local/Packages/Engine.Main)
INCLUDE(${entryPointRoot}/Scripts/HeadlessEntryPointFiles.cmake)

SET(buildScripts
	CMakeLists.txt
	)

SET(sourceFiles
	${ENTRY_POINT_FILES}
	test.cpp
	EntryPoint.cpp #GNARLY_TODO: Add logic to AddExecutable to require this file.
	)

SET(projectName json11.Tests)
PROJECT(${projectName})

AddExecutable(${projectName} ${buildScripts} ${sourceFiles})
AddDependency(${projectName} json11)
AddDependency(${projectName} Engine.Core) #GNARLY_TODO: Add support to module deployment to search dependencies. We shouldn't need to include this here since json11 depends on it but is required to auto deploy dll.
AddDependency(${projectName} EASTL) #GNARLY_TODO: Add support to module deployment to search dependencies. We shouldn't need to include this here since Engine.Core depends on it but is required to auto deploy dll.
AddDependency(${projectName} gtest)

AddAdditionalInclude(${projectName} PRIVATE ${entryPointRoot}/Source)

#Organize solution folders and files within the IDE.
SetSolutionFolder(${projectName} ${EXTERNAL_TEST_SOLUTION_FOLDER})
SOURCE_GROUP(BuildScripts FILES ${buildScripts})
