##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##
########################## Project Settings ####################################

cmake_minimum_required (VERSION 2.8)
project (assimp-worker CXX C)

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")

if(WIN32)
  # on windows, pretty much all dependencies go into the contrib package
find_package(Contrib REQUIRED boost_1_55_0 poco-1.4.6p4 assimp activemq-cpp apr-1.5.1 jansson-2.6)
add_definitions(-DBOOST_ALL_DYN_LINK)
else()
  # other platforms are recommended to install dependencies through their
  # native package management systems
endif(WIN32)

message( STATUS "Prefix path: ${CMAKE_PREFIX_PATH}")

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/../install" CACHE PATH "ASSImp Worker install prefix" FORCE)
	message( STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

############################### Modules ########################################

find_package(CXX11)

find_package(Poco REQUIRED COMPONENTS Zip Net Crypto XML)
if(Poco_FOUND) 
   include_directories(BEFORE ${Poco_INCLUDE_DIR})
   link_directories(${Poco_LIBRARY_DIR})
   message(STATUS "Found Poco includes at ${Poco_INCLUDE_DIR}")
endif(Poco_FOUND)

find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options)
if(Boost_FOUND)
   include_directories(BEFORE ${Boost_INCLUDE_DIR})
   link_directories(${Boost_LIBRARY_DIR})
   message(STATUS "Found Boost includes at ${Boost_INCLUDE_DIR}")
endif(Boost_FOUND)

find_package(ASSIMP)
if(ASSIMP_FOUND)
	include_directories(BEFORE ${ASSIMP_INCLUDE_DIR})
	message(STATUS "Found ASSIMP includes at ${ASSIMP_INCLUDE_DIR}")
endif(ASSIMP_FOUND)

find_package(ActiveMQ-CPP REQUIRED)
include_directories(BEFORE ${ActiveMQ-CPP_INCLUDE_DIR})
include_directories(BEFORE ${APR_INCLUDE_DIR})

find_package(Jansson)
if (JANSSON_FOUND)
	include_directories(${JANSSON_INCLUDE_DIRS})
	link_directories(${JANSSON_LIBRARY_DIR})
endif(JANSSON_FOUND)

############################### Includes #######################################

include_directories(include)

################################ Build #########################################

# Additional windows definitions
# Borrowed from NetVFB
if(WIN32)
	add_definitions(
					-D_WIN32_WINNT=0x0601 # Define minimum windows version to remove vc warnings
					-DNOGDI # Don't let macros defined in wingdi.h rampage through your code
					-MD # Use multithreaded dynamic c runtime - use this in all dependencies as well!
					-W4 # Increase warning output on MSVC
					-wd4512 # Disable Warning C4512: 'SomeClass' : assignment operator could not be generated
					-DFD_SETSIZE=1024 # Increase number of available sockets per process (default 64)
					)
endif(WIN32)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")

# /src
add_subdirectory(src)

################################ Install #######################################

