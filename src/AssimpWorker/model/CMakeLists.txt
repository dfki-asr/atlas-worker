##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(MODEL_HPP_FILES
)

SET(MODEL_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Asset.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Blob.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Folder.cpp
)

set(model_HPP_FILES ${MODEL_HPP_FILES} PARENT_SCOPE)
set(model_SRC_FILES ${MODEL_SRC_FILES} PARENT_SCOPE)
