/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "AssimpImporter.hpp"

namespace AssimpWorker {

	AssimpImporter::AssimpImporter() : 
		importer()
	{
		return;
	}

	AssimpImporter::~AssimpImporter(){
		return;
	}

	const aiScene* AssimpImporter::importSceneFromFile(std::string fileName, Log& log){
		importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true); //remove degenerate polys
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT); //Drop all primitives that aren't triangles
		const aiScene* scene = importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality);
		if (!scene) {
			log.error("Scene not imported: "+fileName);
		}
		const char* errorString = importer.GetErrorString();
		if (strlen(errorString) != 0) {
			// error string is not empty: an error occured.
			log.error(fileName + ": " + errorString);
		}
		return scene;
	}

}
