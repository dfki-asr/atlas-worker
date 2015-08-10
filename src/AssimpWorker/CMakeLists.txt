##
# This file is part of ATLAS. It is subject to the license terms in
# the LICENSE file found in the top-level directory of this distribution.
# (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
# You may not use this file except in compliance with the License.
##
################################ Build ########################################

file(GLOB_RECURSE PUBLIC_API ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_subdirectory(model)
add_subdirectory(storage)
add_subdirectory(import)
add_subdirectory(connection)
add_subdirectory(internal)
add_subdirectory(worker)

SET( MAIN_SRC_FILES	
	main.cpp
)

source_group(Main FILES ${MAIN_SRC_FILES})
source_group(Main FILES ${MAIN_HPP_FILES})
source_group(Import\\Ai REGULAR_EXPRESSION /import/AiImporter/*)
source_group(Import\\AML REGULAR_EXPRESSION /import/AMLImporter/*)
source_group(Import\\Collada REGULAR_EXPRESSION /import/ColladaImporter/*)
source_group(Import FILES ${import_SRC_GRP})
source_group(Model FILES ${model_HPP_FILES})
source_group(Model FILES ${model_SRC_FILES})
source_group(Storage FILES ${storage_HPP_FILES})
source_group(Storage FILES ${storage_SRC_FILES})
source_group(Connection FILES ${connection_HPP_FILES})
source_group(Connection FILES ${connection_SRC_FILES})
source_group(Internal FILES ${internal_HPP_FILES})
source_group(Internal FILES ${internal_SRC_FILES})
source_group(Worker FILES ${worker_HPP_FILES})
source_group(Worker FILES ${worker_SRC_FILES})
set(ALL_HPP_FILES
	${model_HPP_FILES}
	${storage_HPP_FILES}
	${import_HPP_FILES}
	${connection_HPP_FILES}
	${internal_HPP_FILES}
	${worker_HPP_FILES}

)

set(ALL_SRC_FILES
	${MAIN_SRC_FILES}
	${model_SRC_FILES}
	${storage_SRC_FILES}
	${import_SRC_FILES}
	${connection_SRC_FILES}
	${internal_SRC_FILES}	
	${worker_SRC_FILES}
)

add_executable (AssimpWorker ${ALL_SRC_FILES} ${ALL_HPP_FILES} ${PUBLIC_API})


################################ Link #########################################
target_link_libraries(
	AssimpWorker 
	${Poco_LIBRARIES} 
	${ASSIMP_LIBRARY} 
	${APR_LIBRARIES}
	${ActiveMQ-CPP_LIBRARIES}
	${JANSSON_LIBRARY}
	${Boost_LIBRARIES}
)

########################### Install ###########################################

install(TARGETS AssimpWorker DESTINATION bin)

set_target_properties(AssimpWorker PROPERTIES FOLDER "AssimpWorker")
