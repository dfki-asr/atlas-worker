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

	class AiSceneImporter : public Importer {
	public:
		AiSceneImporter(const aiScene* scene, const std::string& pathToFolder, Log& log);
		virtual ~AiSceneImporter();
		virtual void addElementsTo(ATLAS::Model::Folder& root);
		void importSubtreeOfScene(ATLAS::Model::Folder& root, aiNode* startingPoint);
	private:
		const aiScene* scene;
		const std::string pathToFolder;

		void addLightsTo(ATLAS::Model::Folder& folder);
		void addCamerasTo(ATLAS::Model::Folder& folder);
	};

} // End namespace AssimpWorker

