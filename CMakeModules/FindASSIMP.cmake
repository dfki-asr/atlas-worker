# - Try to find Assimp
# Once done, this will define
#
#  ASSIMP_FOUND - system has Assimp
#  ASSIMP_INCLUDE_DIR - the Assimp include directories
#  ASSIMP_LIBRARY - link these to use Assimp

FIND_PATH( ASSIMP_INCLUDE_DIR assimp
  PATHS
  /usr/include
  /usr/local/include
  /opt/local/include
)

FIND_LIBRARY( ASSIMP_LIBRARY assimp
  PATHS
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
)

SET( ASSIMP_FOUND FALSE )

IF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
  SET( ASSIMP_FOUND TRUE )
ENDIF(ASSIMP_INCLUDE_DIR AND ASSIMP_LIBRARY)
