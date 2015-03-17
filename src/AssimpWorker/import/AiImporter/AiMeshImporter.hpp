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

	class AiMeshImporter : public Importer {
	public:
		AiMeshImporter(const aiScene* scene, const std::string& pathToFolder, const aiMesh* mesh, Log& log);
		virtual ~AiMeshImporter();
		virtual void addElementsTo(ATLAS::Model::Folder& root);
	private:
		const aiMesh* mesh;
		const aiScene* scene;
		const std::string& pathToFolder;

		std::string getPrimitiveType();
		void addFacesTo(ATLAS::Model::Folder& meshFolder);
		void addPositionsTo(ATLAS::Model::Folder& meshFolder);
		void addNormalsTo(ATLAS::Model::Folder& meshFolder);
		void addTexcoordsTo(ATLAS::Model::Folder& meshFolder);
		void addMaterialTo(ATLAS::Model::Folder& meshFolder);
	};

} // End namespace AssimpWorker

