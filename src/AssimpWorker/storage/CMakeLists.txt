##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##
SET(STORAGE_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/StorageService.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/JCRStorageService.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/DiskStorageService.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/JsonSerializer.hpp
)

SET(STORAGE_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/StorageService.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/JCRStorageService.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DiskStorageService.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/JsonSerializer.cpp
)

set(storage_HPP_FILES ${STORAGE_HPP_FILES} PARENT_SCOPE)
set(storage_SRC_FILES ${STORAGE_SRC_FILES} PARENT_SCOPE)
