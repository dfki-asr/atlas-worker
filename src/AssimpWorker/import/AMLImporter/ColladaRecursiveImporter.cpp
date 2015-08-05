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

	ColladaRecursiveImporter::ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, Poco::URI pathToWorkingDirectory, ColladaMassagerRegistry& registry) :
		Importer(colladaFileURI.toString(), log),
		pathToWorkingDirectory(pathToWorkingDirectory),
		colladaFileURI(colladaFileURI),
		childImporter(),
		importer(NULL),
		massagerRegistry(registry)
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
		bool needToPurge = colladaFileURI.getFragment() != "";
		ColladaMassager* massager = massagerRegistry.getMassager(colladaFileURI);
		massager->massage();
		localScale = massager->getCurrentUnit();

		if (parentScale != -1 && localScale != parentScale) {
			
			throw Exception("Inconsistent scales used in input files");
		}
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
			AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
			sceneImporter.importSubtreeOfScene(root, startingPoint);
		} else {
			AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
			sceneImporter.addElementsTo(root);
		}
		if (needToPurge){
			Folder& startingPoint = findFolderWithName(root, colladaFileURI.getFragment());
			massager->restoreOriginalNames(startingPoint);
		} else {
			massager->restoreOriginalNames(root);
		}
		std::map<std::string, std::string> externalRefMap = massager->getExternalReferences();
		for (auto exRef : externalRefMap){
			Poco::URI uri(fixRelativeReference(exRef.second));
			ColladaRecursiveImporter* ci = new ColladaRecursiveImporter(uri, log, pathToWorkingDirectory, massagerRegistry, localScale);
			childImporter.push_back(ci);
			Folder& entryPoint = findFolderWithColladaID(root, exRef.first);
			ci->addElementsTo( entryPoint );
		}
		removeColladaIDs(root);

	void ColladaRecursiveImporter::fixScales(ATLAS::Model::Folder& root) {
		ATLAS::Model::Blob* currentFolderTransform = root.getBlobByType("transform");
		aiMatrix4x4 scaledMatrix;
		if (currentFolderTransform) {
			scaledMatrix = *(aiMatrix4x4*)currentFolderTransform->getData();
		}
		
		std::cout << "Before scaling: " << std::endl << "----------------" << std::endl;
		std::cout << scaledMatrix.a1 << " | " << scaledMatrix.a2 << " | " << scaledMatrix.a3 << " | " << scaledMatrix.a4 << std::endl;
		std::cout << scaledMatrix.b1 << " | " << scaledMatrix.b2 << " | " << scaledMatrix.b3 << " | " << scaledMatrix.b4 << std::endl;
		std::cout << scaledMatrix.c1 << " | " << scaledMatrix.c2 << " | " << scaledMatrix.c3 << " | " << scaledMatrix.c4 << std::endl;
		std::cout << scaledMatrix.d1 << " | " << scaledMatrix.d2 << " | " << scaledMatrix.d3 << " | " << scaledMatrix.d4 << std::endl;
		std::cout << "Local scale in fixScales: " << localScale << std::endl;
		aiMatrix4x4 scaling;
		aiVector3t<float> scalingVector(localScale);
		std::cout << scalingVector.x << " | " << scalingVector.y << " | " << scalingVector.z << std::endl;
		aiMatrix4x4::Scaling(aiVector3t<float>(localScale), scaling);
		
		scaledMatrix *= scaling;
		std::cout << "After scaling: " << std::endl << "----------------" << std::endl;
		std::cout << scaledMatrix.a1 << " | " << scaledMatrix.a2 << " | " << scaledMatrix.a3 << " | " << scaledMatrix.a4 << std::endl;
		std::cout << scaledMatrix.b1 << " | " << scaledMatrix.b2 << " | " << scaledMatrix.b3 << " | " << scaledMatrix.b4 << std::endl;
		std::cout << scaledMatrix.c1 << " | " << scaledMatrix.c2 << " | " << scaledMatrix.c3 << " | " << scaledMatrix.c4 << std::endl;
		std::cout << scaledMatrix.d1 << " | " << scaledMatrix.d2 << " | " << scaledMatrix.d3 << " | " << scaledMatrix.d4 << std::endl;
		//DataDeletingBlob<aiMatrix4x4>* localTransformBlob = new DataDeletingBlob<aiMatrix4x4>(scaledMatrix);
		currentFolderTransform->setData(&scaledMatrix, sizeof(scaledMatrix));
		//root.addBlob("transform", *localTransformBlob); */
		
	}

	void ColladaRecursiveImporter::removeColladaIDs(Folder& folder){
		folder.removeAttribute("colladaID");
		for (Folder& child : folder.getChildren()){
			removeColladaIDs(child);
		}
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