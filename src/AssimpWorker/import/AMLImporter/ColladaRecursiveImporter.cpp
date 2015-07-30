/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/

#include <iostream>
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
		colladaFileURI(colladaFileURI),
		childImporter(),
		importer(NULL)
	{
		return;
	}

	ColladaRecursiveImporter::~ColladaRecursiveImporter(){
		delete importer;
		for (auto ci : childImporter){
			delete ci;
		}
	}

	void ColladaRecursiveImporter::addElementsTo(ATLAS::Model::Folder& root){
		std::cout << "ColladaRecursiveImporter:addElementsoTo - importing: " << colladaFileURI.toString() << std::endl;
		bool needToPurge = colladaFileURI.getFragment() != "";
		massager.massage();
		this->importer = new AssimpWorker::AssimpImporter();
		const aiScene* scene = importer->importSceneFromFile(colladaFileURI.getPath(), log);
		if (!scene) {
			return;
		}
		if (needToPurge) {
			aiNode* startingPoint = findaiNodeWithName(scene->mRootNode, colladaFileURI.getFragment());
			if (startingPoint == NULL){
				throw AMLException("Could not find a Node with id '" + colladaFileURI.getFragment() + "'");
			}
			massager.restoreOriginalNames(startingPoint);
			AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
			sceneImporter.importSubtreeOfScene(root, startingPoint);
		} else {
			AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
			sceneImporter.addElementsTo(root);
		}
		std::map<std::string, std::string> externalRefMap = massager.getExternalReferences();
		for (auto exRef : externalRefMap){
			Poco::URI uri(fixRelativeReference(exRef.second));
			ColladaRecursiveImporter* ci = new ColladaRecursiveImporter(uri, log, pathToWorkingDirectory);
			childImporter.push_back(ci);
			//TODO the shit with the childfolders
			ci->addElementsTo(root);//, findFolderWithName(root, exRef.first));
		}
	}

	std::string ColladaRecursiveImporter::fixRelativeReference(std::string relativeURIasString){
		std::string pathOfParentfile = colladaFileURI.getPath();
		auto lastSlashIndex = pathOfParentfile.find_last_of('/');
		pathOfParentfile.erase(lastSlashIndex+1);
		pathOfParentfile.append(relativeURIasString);
		return pathOfParentfile;
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