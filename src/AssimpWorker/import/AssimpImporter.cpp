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
#include "../internal/configuration.hpp"

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
		setOptions();
		const aiScene* scene = importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_MaxQuality);
		if (!scene) {
			log.error("Scene not imported: "+fileName);
		}
		log.error(importer.GetErrorString());
		return scene;
	}

	void AssimpImporter::setOptions()
	{
		Configuration& config = Configuration::getInstance();
		importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true); //remove degenerate polys
		importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT); //Drop all primitives that aren't triangles
		if (config.enabled("mesh-split")) {
			int threshold = config.get("mesh-split-threshold").as<int>();
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, threshold);
		}
	}

}
