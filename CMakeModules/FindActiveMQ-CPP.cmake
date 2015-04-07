# - Find ActiveMQ-CPP 
#
# Once done this will define
#  ActiveMQ-CPP_FOUND - System has APR 
#  ActiveMQ-CPP_INCLUDE_DIRS - The APR include directories
#  ActiveMQ-CPP_LIBRARIES - The libraries needed to use APR 
#  ActiveMQ-CPP_DEFINITIONS - Compiler switches required for using APR

include(LibFindMacros)

libfind_package(ActiveMQ-CPP APR)
libfind_pkg_check_modules(ActiveMQ-CPP_PKGCONF activemq-cpp)

if(PKG_CONFIG_EXECUTABLE)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "--modversion" "activemq-cpp"
                    OUTPUT_VARIABLE ActiveMQ-CPP_PKGCONFIG_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(ActiveMQ-CPP_INCLUDE_HINT /usr/local/include/activemq-cpp-${ActiveMQ-CPP_PKGCONFIG_VERSION}/)
endif(PKG_CONFIG_EXECUTABLE)

find_path(ActiveMQ-CPP_INCLUDE_DIR cms/Config.h
          DOC "ActiveMQ-CPP include files"
          HINTS ${ActiveMQ-CPP_PKGCONF_INCLUDE_DIRS}
                # workaround for .pc file with wrong dependency
                ${ActiveMQ-CPP_INCLUDE_HINT})

find_library(ActiveMQ-CPP_LIBRARY NAMES activemq-cpp
             DOC "ActiveMQ-CPP library file"
             HINTS ${ActiveMQ-CPP_PKGCONF_LIBRARY_DIRS} )

libfind_process(ActiveMQ-CPP)
