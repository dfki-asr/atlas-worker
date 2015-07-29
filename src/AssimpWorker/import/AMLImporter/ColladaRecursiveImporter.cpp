/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/

#include "ColladaRecursiveImporter.hpp"
#include "../../internal/Exception.hpp"
#include "../AiImporter/AiSceneImporter.hpp"

namespace AssimpWorker {

	using ATLAS::Model::DataDeletingBlob;
	using ATLAS::Model::Folder;

	ColladaRecursiveImporter::ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, Poco::URI pathToWorkingDirectory) :
		Importer(colladaFileURI.toString(), log),
		pathToWorkingDirectory(pathToWorkingDirectory),
		massager(colladaFileURI),
		colladaFileURI(colladaFileURI)
	{
		return;
	}

	ColladaRecursiveImporter::~ColladaRecursiveImporter(){
		delete importer;
	}

	void ColladaRecursiveImporter::addElementsTo(ATLAS::Model::Folder& root){
	};

	void ColladaRecursiveImporter::recursiveColladaImport(Folder& root){
		massager.purgeNames();
		importer = new AssimpWorker::AssimpImporter();
		const aiScene* scene = importer->importSceneFromFile(colladaFileURI.getPath(), log);
		if (!scene) {
			return;
		}
		aiNode* startingPoint = scene->mRootNode;
		if (colladaFileURI.getFragment() != "") {
			aiNode* startingPoint = findaiNodeWithName(scene->mRootNode, colladaFileURI.getFragment());
			if (startingPoint == NULL){
				throw AMLException("Could not find a Node with id '" + colladaFileURI.getFragment() + "'");
			}
		}
		restoreOriginalNames(startingPoint);
		AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
		sceneImporter.importSubtreeOfScene(root, startingPoint);
	}

	void ColladaRecursiveImporter::restoreOriginalNames(aiNode* node) {
		node->mName = massager.getNameForId(node->mName.C_Str());
		for (int i = 0; i < node->mNumChildren; i++){
			restoreOriginalNames(node->mChildren[i]);
		}
	}

	aiNode* ColladaRecursiveImporter::findaiNodeWithName(aiNode* node, const std::string& name){
		if (name == node->mName.C_Str()){
			return node;
		}
		aiNode* childNode = NULL;
		for (int i = 0; i < node->mNumChildren; i++){
			childNode = findaiNodeWithName(node->mChildren[i], name);
			if (childNode != NULL) {
				break;
			}
		}
		return childNode;
	}
}