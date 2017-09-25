CMAKE_MINIMUM_REQUIRED (VERSION 3.7.0)

INCLUDE(${CMAKE_SOURCE_DIR}/Local/Packages/Engine.CMake.Scripts/ModuleMethods.cmake)

InitModule()

#Source files for the library
INCLUDE(Scripts/CodeFiles.cmake)

SET(projectName json11)
AddProject(${projectName})

AddLibrary(${projectName}
	${CODE_FILES}
	)

AddSharedDefinitions(${projectName} JSON11_EXPORT)
AddDependency(${projectName} Engine.Core)
AddAdditionalInclude(${projectName} PUBLIC "Public")
AddAdditionalInclude(${projectName} PRIVATE "Private")

#Organize solution folders and files within the IDE.
SetSolutionFolder(${projectName} ${EXTERNAL_LIBRARY_SOLUTION_FOLDER})

#Add the unit tests for this module
ADD_SUBDIRECTORY(Tests)
