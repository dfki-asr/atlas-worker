##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##

SET(CONNECTION_HPP_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Connection.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/EasyRequest.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/FeedbackProducer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/QueueConsumer.hpp
)

SET(CONNECTION_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Connection.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/EasyRequest.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/FeedbackProducer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/QueueConsumer.cpp
)

set(connection_HPP_FILES ${CONNECTION_HPP_FILES} PARENT_SCOPE)
set(connection_SRC_FILES ${CONNECTION_SRC_FILES} PARENT_SCOPE)
