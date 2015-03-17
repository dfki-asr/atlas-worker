##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(WORKER_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Worker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/WorkUnit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/zipFileDecompressor.hpp
)

SET(WORKER_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Worker.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WorkUnit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zipFileDecompressor.cpp
)

set(worker_HPP_FILES ${WORKER_HPP_FILES} PARENT_SCOPE)
set(worker_SRC_FILES ${WORKER_SRC_FILES} PARENT_SCOPE)
