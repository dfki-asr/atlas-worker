/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "assimp/scene.h"
#include "../Importer.hpp"

namespace AssimpWorker {

	class AiNodeImporter : public Importer {
	public:
		AiNodeImporter(const aiScene* scene, const std::string& pathToFolder, aiNode* node, Log& log);
		virtual ~AiNodeImporter();
		virtual void addElementsTo(ATLAS::Model::Folder& root);
	private:
		const aiScene* scene;
		aiNode* node;
		const std::string pathToFolder;
		void addMeshesTo(ATLAS::Model::Folder& nodeFolder);
		void addTransformsTo(ATLAS::Model::Folder& nodeFolder, ATLAS::Model::Folder& root);
		void addChildrenTo(ATLAS::Model::Folder& nodeFolder);
	};

} // End namespace AssimpWorker

