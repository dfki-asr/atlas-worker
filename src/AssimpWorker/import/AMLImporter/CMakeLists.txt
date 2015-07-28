##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(IMPORT_AML_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AMLImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AMLFrameImporter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ColladaMassager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ColladaRecursiveImporter.hpp
)

SET(IMPORT_AML_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AMLImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AMLFrameImporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ColladaMassager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ColladaRecursiveImporter.cpp
)

set(import_AML_HPP_FILES ${IMPORT_AML_HPP_FILES} PARENT_SCOPE)
set(import_AML_SRC_FILES ${IMPORT_AML_SRC_FILES} PARENT_SCOPE)
