##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(IMPORT_AI_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AiMaterialImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AiNodeImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AiMeshImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AiSceneImporter.hpp
)

SET(IMPORT_AI_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AiMaterialImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AiNodeImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AiMeshImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AiSceneImporter.cpp
)

set(import_AI_HPP_FILES ${IMPORT_AI_HPP_FILES} PARENT_SCOPE)
set(import_AI_SRC_FILES ${IMPORT_AI_SRC_FILES} PARENT_SCOPE)
