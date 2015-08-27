##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(INTERNAL_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AssimpVersion.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/configuration.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Exception.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Iterator.hpp
)

include(GetGitRevisionDescription)
git_describe(VERSION --always --tags --dirty=-dirty)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp)

SET(INTERNAL_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AssimpVersion.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Exception.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Version.cpp
)

set(internal_HPP_FILES ${INTERNAL_HPP_FILES} PARENT_SCOPE)
set(internal_SRC_FILES ${INTERNAL_SRC_FILES} PARENT_SCOPE)
