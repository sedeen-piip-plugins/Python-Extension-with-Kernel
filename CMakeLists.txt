CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( PythonExtension_kernel )

##
## Load the Sedeen dependencies
FIND_PACKAGE( SEDEENSDK REQUIRED
                HINTS ../../..
                "$ENV{PROGRAMFILES(X86)}/Sedeen Viewer SDK/v5.2.1.427/msvc2012"
                "$C/Azadeh/Sedeen Viewer SDK/v5.2.1.427/msvc2012" )

find_package(Python2 COMPONENTS Interpreter Development NumPy)


INCLUDE_DIRECTORIES( "${SEDEENSDK_INCLUDE_DIR}" )

LINK_DIRECTORIES( "${SEDEENSDK_LIBRARY_DIR}" )

##
## Build the code into a module library
ADD_LIBRARY( PythonExtension_kernel SHARED
	PythonEmbeddingWrapper.cpp
	PythonEmbeddingWrapper.h
	PythonException.cpp
	PythonException.h
	PythonExtensionWithKernel.cpp
	PythonExtensionWithKernel.h
	PythonKernel.cpp
	PythonKernel.h
	PythonObject.cpp
	PythonObject.h)

target_compile_features(PythonExtension_kernel PUBLIC cxx_std_17)

# Link the library against the Sedeen libraries
# NOTE: The QT libraries must be linked first.
TARGET_LINK_LIBRARIES( PythonExtension_kernel ${SEDEENSDK_LIBRARIES} )

TARGET_LINK_LIBRARIES(PythonExtension_kernel Python2::Python Python2::NumPy)

##
## Install the plugin in the sedeen plugins directory
IF( ${PATHCORE_FOUND} )
  INSTALL( TARGETS PythonExtension_kernel
           LIBRARY DESTINATION "${PATHCORE_DIR}/plugins" )
ENDIF()
