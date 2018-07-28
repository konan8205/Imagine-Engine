
# ------------------------------
# 
# 		Setup Project
# 
# ------------------------------

cmake_minimum_required(VERSION 3.8)

# Project: Imagine Engine
set(PROJECT_NAME "IE")
project(${PROJECT_NAME})

message(STATUS "Using CMake to setup Imagine Engine build...\n")

# Default build option
if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release ... FORCE )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DUNICODE)
endif()

# ------------------------------
# 
# 		Setup Vulkan SDK
# 
# ------------------------------

#
# Vulkan SDK includes and libraries
#
message(STATUS "Attempting locate Vulkan SDK using CMake...")

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
message(STATUS "Vulkan include directory is " ${Vulkan_INCLUDE_DIRS})
link_libraries(${Vulkan_LIBRARIES})
message(STATUS "Vulkan library directory is " ${Vulkan_LIBRARIES} "\n")


#
# Required preprocessor definitions
#
if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
	add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "IOS")
	add_definitions(-DVK_USE_PLATFORM_IOS_MVK)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Mac OS X")
	add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)

# Nintendo Switch: Unsupported platform
#add_definitions(-DVK_USE_PLATFORM_VI_NN)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# Deprecated library
	#add_definitions(-DVK_USE_PLATFORM_MIR_KHR)
	add_definitions(-DVK_USE_PLATFORM_WAYLAND_KHR)
	add_definitions(-DVK_USE_PLATFORM_XCB_KHR)
	add_definitions(-DVK_USE_PLATFORM_XLIB_KHR)
	add_definitions(-DVK_USE_PLATFORM_XLIB_XRANDR_EXT)

else()
	message(FATAL_ERROR "Unsupported platform detected.")

endif()


# ------------------------------
#
# 	Setup OpenGL Helper Libraries
#
# ------------------------------

#
# GLM Setup - Mathematic libraries
#
set(EXTDIR "${CMAKE_SOURCE_DIR}/External/glm")
set(GLM_INCLUDE "${EXTDIR}")
get_filename_component(GLM_INCLUDE_PREFIX "${GLM_INCLUDE}" ABSOLUTE)
if(EXISTS ${GLM_INCLUDE_PREFIX})
	message(STATUS "Found glm header: ${GLM_INCLUDE}")

else()
	message(FATAL_ERROR "Necessary glm headers do not exist: ${GLM_INCLUDE_PREFIX}\n")
	
endif()

include_directories(${GLM_INCLUDE_PREFIX})

#
# GLI Setup - Image library
#
set (EXTDIR "${CMAKE_SOURCE_DIR}/External/gli")
set (GLI_INCLUDE "${EXTDIR}")
get_filename_component(GLI_INCLUDE_PREFIX "${GLI_INCLUDE}" ABSOLUTE)
if(EXISTS ${GLI_INCLUDE_PREFIX})
	message(STATUS "Found gli header: ${GLI_INCLUDE}\n")

else()
	message(FATAL_ERROR "Necessary gli headers do not exist: ${GLI_INCLUDE_PREFIX}\n")
	
endif()

include_directories(${GLI_INCLUDE_PREFIX})


# ------------------------------
#
# 		Setup Sources
#
# ------------------------------

# Define include path
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# Gather list of header and source files for compilation
file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
file(GLOB_RECURSE CXX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cxx)
file(GLOB_RECURSE HPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp)
file(GLOB_RECURSE H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.*)

# https://stackoverflow.com/questions/31422680
if(MSVC)
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
	message(STATUS "Defining directories and files for MSVC...\n")
	assign_source_group(${SOURCE_FILES})
	
endif()

# Build project, give it a name and includes list of file to be compiled
add_executable(${PROJECT_NAME} ${HPP_FILES} ${H_FILES} ${CPP_FILES} ${CXX_FILES})

# Link the debug and release libraries to the project
target_link_libraries( ${PROJECT_NAME} ${VULKAN_LIB_LINK_LIST} )

# Define C++ version to be used for building the project
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

# Define C version to be used for building the project
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)

# Define project properties
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/Bin)
	if (MSVC)
		# Since CMake 3.8
		set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Bin")

	endif()

else()
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR}/Bin32)
	if (MSVC)
		set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/Bin32")

	endif()

endif()