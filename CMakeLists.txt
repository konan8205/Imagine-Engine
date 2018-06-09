
cmake_minimum_required(VERSION 3.8)

# Project Name
set(Project_Name "IE")
project(${Project_Name})

if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release ... FORCE )
endif()

# ------------------------------
# 		Setup Vulkan SDK
# ------------------------------

# Add any required preprocessor definitions here
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
endif()

# Vulkan PATH Settings
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	option(VULKAN_AUTO_LOCATE "VULKAN_AUTO_LOCATE" ON)
	set(VULKAN_PATH "C:/VulkanSDK/1.*/" CACHE STRING "")
else()
	option(VULKAN_AUTO_LOCATE "VULKAN_AUTO_LOCATE" OFF)
	set(VULKAN_PATH "/usr/share/VulkanSDK/1.*/" CACHE STRING "")
endif()

#
# Vulkan PATH Auto Locator
#
if(VULKAN_AUTO_LOCATE)
	
	message(STATUS "Attempting auto locate Vulkan using CMake......")
	
	# Find Vulkan Path using CMake's Vulkan Module
	# This will return Boolean 'Vulkan_FOUND' indicating the status of find as success(ON) or fail(OFF).
	# Include directory path - 'Vulkan_INCLUDE_DIR' and 'Vulkan_LIBRARY' with required libraries.
	find_package(Vulkan)
	
	# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIR}
	if (NOT ${Vulkan_INCLUDE_DIR} STREQUAL "")
		set(VULKAN_PATH ${Vulkan_INCLUDE_DIR})
		if(WIN32)
			STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
		else()
			STRING(REGEX REPLACE "/source/include" "" VULKAN_PATH ${VULKAN_PATH})
		endif()
	endif()
	
	if(NOT Vulkan_FOUND)
		# CMake may fail to locate the libraries but could be able to 
		# provide some path in Vulkan SDK include directory variable
		# 'Vulkan_INCLUDE_DIR', try to extract path from this.
		message(STATUS "Failed to locate Vulkan SDK, retrying again...")
		if(EXISTS "${VULKAN_PATH}")
			message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
		else()
			message("Error: Unable to locate Vulkan SDK.")
			return()
		endif()
	endif()
	
#
# Vulkan PATH Manual Locator
#
else()
	message(STATUS "Attempting to locate Vulkan SDK using manual path......")
	message(STATUS "Using manual specified path: ${VULKAN_PATH}")
	
	# Check if manual set path exists
	if(NOT EXISTS "${VULKAN_PATH}")
		message("Error: Unable to locate this Vulkan SDK path: ${VULKAN_PATH}")
		return()
	endif()
endif()

# We do not use ${Vulkan_LIBRARY}, instead we specify as per our need.
# Add 'vulkan-1' or 'libvulkan' library for building Vulkan applications.
if(WIN32)
	set(VULKAN_LIB_LINK_LIST "vulkan-1.lib")
else()
	set(VULKAN_LIB_LINK_LIST "libvulkan.so")
endif()

# Link directory for vulkan-1 or libvulkan
if(WIN32)
	if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
		link_directories(${VULKAN_PATH}/Bin;${VULKAN_PATH}/Lib;)
	else( CMAKE_SIZEOF_VOID_P EQUAL 8 ) 
		link_directories(${VULKAN_PATH}/Bin32;${VULKAN_PATH}/Lib32;)
	endif()
else()
	link_directories(${VULKAN_PATH}/source/bin;${VULKAN_PATH}/source/lib;)
endif()

if(BUILD_SPV_ON_COMPILE_TIME)
	
	# Preprocessor  flag allows the solution to use glslang library functions
	add_definitions("-DAUTO_COMPILE_GLSL_TO_SPV")

	# GLSL - use Vulkan SDK's glslang library for compling GLSL to SPV 
	# This does not require offline coversion of GLSL shader to SPIR-V(.spv) form 
	if(WIN32)
		set(GLSLANGDIR "${VULKAN_PATH}/glslang")
	else()
		set(GLSLANGDIR "${VULKAN_PATH}/source/glslang")
	endif()
	
	get_filename_component(GLSLANG_PREFIX "${GLSLANGDIR}" ABSOLUTE)
	if(NOT EXISTS ${GLSLANG_PREFIX})
		message(FATAL_ERROR "Error: Necessary glslang components do not exist: " ${GLSLANG_PREFIX})
	endif()
	include_directories( ${GLSLANG_PREFIX} )
	
	# If compiling GLSL to SPV using we need the following libraries
	set(GLSLANG_LIBS SPIRV glslang OGLCompiler OSDependent HLSL)

	# Generate the list of files to link, per flavor.
	foreach(x ${GLSLANG_LIBS})
		list(APPEND VULKAN_LIB_LINK_LIST debug ${x}d optimized ${x})
	endforeach()
	
	# Note: While configuring CMake for glslang we created the binaries in a "build" folder inside ${VULKAN_PATH}/glslang.
	# Therefore, you must edit the below lines for your custorm path like <Your binary path>/OGLCompilersDLL , <Your binary path>/OSDependent/Windows
	link_directories(${GLSLANGDIR}/build/OGLCompilersDLL )
	link_directories(${GLSLANGDIR}/build/glslang/OSDependent/Windows)
	link_directories(${GLSLANGDIR}/build/glslang)
	link_directories(${GLSLANGDIR}/build/SPIRV)
	link_directories(${GLSLANGDIR}/build/hlsl)
endif()


# ------------------------------
# 	Setup Additional Libraries
# ------------------------------

# GLM Setup - Mathematic libraries
set(EXTDIR "${CMAKE_SOURCE_DIR}/External/glm")
set(GLM_INCLUDE "${EXTDIR}")
get_filename_component(GLM_INCLUDE_PREFIX "${GLM_INCLUDE}" ABSOLUTE)
if(NOT EXISTS ${GLM_INCLUDE_PREFIX})
    message(FATAL_ERROR "Error: Necessary glm headers do not exist: " ${GLM_INCLUDE_PREFIX})
endif()
include_directories(${GLM_INCLUDE_PREFIX})

# GLI Setup - Image library
set (EXTDIR "${CMAKE_SOURCE_DIR}/External/gli")
set (GLI_INCLUDE "${EXTDIR}")
get_filename_component(GLI_INCLUDE_PREFIX "${GLI_INCLUDE}" ABSOLUTE)
if(NOT EXISTS ${GLI_INCLUDE_PREFIX})
    message(FATAL_ERROR "Error: Necessary gli headers do not exist: ${GLI_INCLUDE_PREFIX}")
endif()
include_directories(${GLI_INCLUDE_PREFIX})


# ------------------------------
# 	Setup Sources
# ------------------------------

# Include Vulkan header files from Vulkan SDK
include_directories(${VULKAN_PATH}/Include)
include_directories(${VULKAN_PATH}/Source/include)

# Define include path
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Gather list of header and source files for compilation
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
file(GLOB_RECURSE CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cxx)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp)
file(GLOB_RECURSE H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.*)

# Define directories and the contained folder and files inside.
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${SOURCE_FILES})

# Build project, give it a name and includes list of file to be compiled
add_executable(${Project_Name} ${HPP_FILES} ${H_FILES} ${CPP_FILES} ${CXX_FILES})

# Link the debug and release libraries to the project
target_link_libraries( ${Project_Name} ${VULKAN_LIB_LINK_LIST} )

# Define C++ version to be used for building the project
set_property(TARGET ${Project_Name} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${Project_Name} PROPERTY CXX_STANDARD_REQUIRED ON)

# Define C version to be used for building the project
set_property(TARGET ${Project_Name} PROPERTY C_STANDARD 11)
set_property(TARGET ${Project_Name} PROPERTY C_STANDARD_REQUIRED ON)

# Define project properties
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	if (WIN32)
		# Since CMake 3.8
		set_target_properties(${Project_Name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Bin")
	endif()
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${Project_Name} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	if (WIN32)
		# Since CMake 3.8
		set_target_properties(${Project_Name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Bin32")
	endif()
endif()