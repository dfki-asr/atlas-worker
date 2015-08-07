/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/

#include <iostream>
#include "ColladaRecursiveImporter.hpp"
#include "../../internal/Exception.hpp"

namespace AssimpWorker {

	using ATLAS::Model::DataDeletingBlob;
	using ATLAS::Model::Folder;

	ColladaRecursiveImporter::ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, Poco::URI pathToWorkingDirectory, ColladaMassagerRegistry& registry, float scale) :
		Importer(colladaFileURI.toString(), log),
		pathToWorkingDirectory(pathToWorkingDirectory),
		colladaFileURI(colladaFileURI),
		childImporter(),
		importer(NULL),
		massagerRegistry(registry),
		massager(NULL),
		localScale(0),
		parentScale(scale),
		recursionDepth(0)
	{
		return;
	}

	ColladaRecursiveImporter::~ColladaRecursiveImporter(){
		delete importer;
		for (auto ci : childImporter){
			delete ci;
		}
	}

	void ColladaRecursiveImporter::addElementsTo(Folder& root){
		preprocessCollada();
		const aiScene* scene = runAssimpImport();
		convertToFolderStructure(scene, root);
		importChildColladas(root);
	}

	void ColladaRecursiveImporter::preprocessCollada(){
		massager = massagerRegistry.getMassager(colladaFileURI);
		massager->massage();
		localScale = massager->getCurrentUnit();
		colladaUpAxis = massager->getUpAxis();
		if (parentScale != -1 && localScale != parentScale) {
			throw Exception("Inconsistent scales used in input files.");
		}
	}

	const aiScene* ColladaRecursiveImporter::runAssimpImport(){
		this->importer = new AssimpWorker::AssimpImporter();
		const aiScene* scene = importer->importSceneFromFile(colladaFileURI.getPath(), log);
		if (!scene) {
			throw Exception("Error while running Assimp.");
		}
		return scene;
	}

	void ColladaRecursiveImporter::convertToFolderStructure(const aiScene* scene, Folder& root){
		if (colladaFileURI.getFragment() != "") {
			aiNode* startingPointToImportFrom = findaiNodeWithName(scene->mRootNode, colladaFileURI.getFragment());
			if (startingPointToImportFrom == NULL){
				throw AMLException("Could not find a Node with id '" + colladaFileURI.getFragment() + "'");
			}
			AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
			sceneImporter.importSubtreeOfScene(root, startingPointToImportFrom);
			Folder& startingPointToRestoreNames = findFolderWithName(root, colladaFileURI.getFragment());
			massager->restoreOriginalNames(startingPointToRestoreNames);
		}
		else {
			AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
			sceneImporter.addElementsTo(root);
			massager->restoreOriginalNames(root);
		}
	}

	void ColladaRecursiveImporter::importChildColladas(Folder& root){
		auto externalRefMap = massager->getExternalReferences();
		for (auto exRef : externalRefMap){
			Poco::URI uri(fixRelativeReference(exRef.second));
			ColladaRecursiveImporter* ci = new ColladaRecursiveImporter(uri, log, pathToWorkingDirectory, massagerRegistry, localScale);
			childImporter.push_back(ci);
			Folder& entryPoint = findFolderWithColladaID(root, exRef.first);
			ci->addElementsTo(entryPoint);
		}
	}

	const std::string ColladaRecursiveImporter::getColladaUpAxis(){
		return colladaUpAxis;
	}

	const float ColladaRecursiveImporter::getLocalScale() {
		return localScale;
	}

	std::string ColladaRecursiveImporter::fixRelativeReference(std::string relativeURIasString){
		std::string pathOfParentfile = colladaFileURI.getPath();
		auto lastSlashIndex = pathOfParentfile.find_last_of('/');
		pathOfParentfile.erase(lastSlashIndex+1);
		pathOfParentfile.append(relativeURIasString);
		return pathOfParentfile;
	}

	Folder& ColladaRecursiveImporter::findFolderWithName(Folder& folder, std::string name){
		if (name == folder.getName()){
			return folder;
		}
		std::vector<Folder>& children = folder.getChildren();
		Folder* found = NULL;
		for (Folder& child : children){
			found = &(findFolderWithName(child, name));
			if (found != NULL){
				break;
			}
		}
		return *found;
	}

	Folder& ColladaRecursiveImporter::findFolderWithColladaID(Folder& folder, std::string id){
		if (id == (folder).getAttribute("colladaID")){
			return folder;
		}
		std::vector<Folder>& children = folder.getChildren();
		Folder* found = NULL;
		for (Folder& child : children){
			found = &(findFolderWithColladaID(child, id));
			if (found != NULL){
				break;
			}
		}
		return *found;
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