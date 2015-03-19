# - Try to find Assimp
# Once done, this will define
#
#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIR - the Assimp include directories
#  ASSIMP_LIBRARY - link these to use Assimp

include(LibFindMacros)

libfind_pkg_check_modules(ASSIMP_PKGCONF assimp)

find_path(ASSIMP_INCLUDE_DIR assimp/scene.h
          DOC "ASSIMP include file"
          HINTS ${ASSIMP_PKGCONF_INCLUDE_DIRS})

find_library(ASSIMP_LIBRARY NAMES assimp
             DOC "ASSIMP library file"
             HINTS ${ASSIMP_PKGCONF_LIBRARY_DIRS} )

libfind_process(ASSIMP)
