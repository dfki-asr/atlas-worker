/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include "AiNodeImporter.hpp"
#include "AiMeshImporter.hpp"
#include "../../internal/Exception.hpp"
#include <atlas/model/Blob.hpp>

namespace AssimpWorker {

	using ATLAS::Model::Folder;
	using ATLAS::Model::Blob;
	using ATLAS::Model::DataDeletingBlob;
	using ATLAS::Model::DataUntouchingBlob;

	/////////////////////////// AiNodeImporter

	AiNodeImporter::AiNodeImporter(const aiScene* scene, const std::string& pathToFolder, aiNode* node, Log& log) :
		Importer(pathToFolder, log),
		scene(scene),
		node(node),
		pathToFolder(pathToFolder)
	{
		return;
	}

	AiNodeImporter::~AiNodeImporter(){
		return;
	}

	void AiNodeImporter::addElementsTo(Folder& root){
		Folder& nodeFolder = root.appendChild("node");
		nodeFolder.setName(node->mName.C_Str());
		addTransformsTo(nodeFolder, root);
		addMeshesTo(nodeFolder);
		addChildrenTo(nodeFolder);
	}

	void AiNodeImporter::addChildrenTo(Folder& nodeFolder) {
		for (unsigned int i = 0; i < node->mNumChildren; i++){
			AiNodeImporter childImporter(scene, pathToFolder, node->mChildren[i], log);
			childImporter.addElementsTo(nodeFolder);
		}
	}

	void AiNodeImporter::addMeshesTo(Folder& nodeFolder) {
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			AiMeshImporter meshImporter(scene, pathToFolder, mesh, log);
			meshImporter.addElementsTo(nodeFolder);
		}
	}

	void AiNodeImporter::addTransformsTo(Folder& nodeFolder, Folder& root) {
		DataUntouchingBlob transformBlob(&node->mTransformation, sizeof(aiMatrix4x4));
		nodeFolder.addBlob("transform", transformBlob);
	}

} // End namespace AssimpWorker
