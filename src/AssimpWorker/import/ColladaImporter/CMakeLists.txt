##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(IMPORT_COLLADA_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/ColladaImporter.hpp
)

SET(IMPORT_COLLADA_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/ColladaImporter.cpp
)

set(import_COLLADA_HPP_FILES ${IMPORT_COLLADA_HPP_FILES} PARENT_SCOPE)
set(import_COLLADA_SRC_FILES ${IMPORT_COLLADA_SRC_FILES} PARENT_SCOPE)
