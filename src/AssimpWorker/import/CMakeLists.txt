##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

add_subdirectory(AiImporter)
add_subdirectory(AMLImporter)

SET(IMPORT_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AssimpImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/FileImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Importer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Log.hpp
)

SET(IMPORT_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AssimpImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/FileImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Importer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Log.cpp
)

set(import_SRC_GRP ${IMPORT_HPP_FILES}
		   ${IMPORT_SRC_FILES} PARENT_SCOPE)
set(import_HPP_FILES ${IMPORT_HPP_FILES}
		     ${import_AI_HPP_FILES}
		     ${import_AML_HPP_FILES} PARENT_SCOPE)

set(import_SRC_FILES ${IMPORT_SRC_FILES}
		     ${import_AI_SRC_FILES}
		     ${import_AML_SRC_FILES} PARENT_SCOPE)

