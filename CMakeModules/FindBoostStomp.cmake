# - Try to find Assimp
# Once done, this will define
#
#  BoostStomp_FOUND - system has BoostStomp
#  BoostStomp_INCLUDE_DIR - the BoostStomp include directories
#  BoostStomp_LIBRARY - link these to use BoostStomp

FIND_PATH( BoostStomp_INCLUDE_DIR BoostStomp.hpp
  PATHS
  /usr/include
  /usr/local/include
  /opt/local/include
)

FIND_LIBRARY( BoostStomp_LIBRARY booststomp
  PATHS
  /usr/lib64
  /usr/lib
  /usr/local/lib
  /opt/local/lib
)

SET( BoostStomp_FOUND FALSE )

IF(BoostStomp_INCLUDE_DIR AND BoostStomp_LIBRARY)
  SET( BoostStomp_FOUND TRUE )
ENDIF(BoostStomp_INCLUDE_DIR AND BoostStomp_LIBRARY)
