# - a generic contrib module
#
# find modules inside a contrib path
#
# modules are expected to have been installed into the
# contrib dir:
# /contrib
#     /${platform}
#         /contrib-component
#             /lib
#             /include
#             /bin
#
# if you specify COMPONENTS for this find module,
# it will set Contrib_${component}_DIR to the directory
# inside the contrib dir, as well as add that directory
# to CMAKE_PREFIX_PATH
#

message(STATUS "Looking for Contrib")
if (NOT Contrib_DIR)
  message(STATUS "Contrib path not defined, using default")
  set(Contrib_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../contrib"
    CACHE FILEPATH
    "Base directory for our Contrib repository where all external libraries are kept."
  )
endif(NOT Contrib_DIR)

if(${Contrib_FIND_REQUIRED} AND NOT IS_DIRECTORY ${Contrib_DIR})
  message(FATAL_ERROR "Contrib directory (${Contrib_DIR}) not found, but required") 
  set(Contrib_DIR NOTFOUND)
endif(${Contrib_FIND_REQUIRED} AND NOT IS_DIRECTORY ${Contrib_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(Contrib_DETECTED_PLATFORM "x64_win_msvc120")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(Contrib_DETECTED_PLATFORM "x64_linux_gcc")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(Contrib_DETECTED_PLATFORM "x64_macosx_llvm")
else()
    set(Contrib_DETECTED_PLATFORM NOTFOUND)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
message(STATUS "Detected Contrib platform: ${Contrib_DETECTED_PLATFORM}")

set(Contrib_FOUND FALSE)

IF(Contrib_FIND_COMPONENTS)
  FOREACH(component ${Contrib_FIND_COMPONENTS})
    message(STATUS "Checking for contrib component: ${component}")
    FIND_PATH(Contrib_${component}_DIR ${component} PATHS "${Contrib_DIR}/${Contrib_DETECTED_PLATFORM}" NO_DEFAULT_PATH)
    if(NOT Contrib_${component}_DIR)
      SET(Contrib_${component}_FOUND 0)
      IF(Contrib_FIND_REQUIRED_${component})
        MESSAGE(FATAL_ERROR "CONTRIB ${component} not available.")
      ENDIF()
    ELSE()
      set(Contrib_${component}_DIR ${Contrib_${component}_DIR}/${component})
      message(STATUS "${component} found at ${Contrib_${component}_DIR}")
      SET(Contrib_${component}_FOUND 1)
    ENDIF()
  ENDFOREACH(component)
else()
  message(STATUS "No contrib components requested")
ENDIF()

if(Contrib_DIR AND Contrib_DETECTED_PLATFORM)
  message(STATUS "Found Contrib at: ${Contrib_DIR}")
  set(Contrib_FOUND TRUE)
  set(Contrib_PLATFORM
    ${Contrib_DETECTED_PLATFORM} 
    CACHE STRING
    "Platform to use for Contrib libraries"
  )
endif(Contrib_DIR AND Contrib_DETECTED_PLATFORM)

if(Contrib_FOUND)
  FOREACH(component ${Contrib_FIND_COMPONENTS})
    if(Contrib_${component}_FOUND)
      LIST(APPEND CMAKE_PREFIX_PATH ${Contrib_${component}_DIR})
    endif(Contrib_${component}_FOUND)
  ENDFOREACH()
endif(Contrib_FOUND)
