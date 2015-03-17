/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#define _USE_MATH_DEFINES

#include "AMLImporter.hpp"
#include "../AiImporter/AiSceneImporter.hpp"
#include "../../internal/Exception.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/Node.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/SAX/InputSource.h>
#include <boost/algorithm/string.hpp>

namespace AssimpWorker {

	using ATLAS::Model::DataDeletingBlob;
	using ATLAS::Model::Folder;

	AMLImporter::AMLImporter(const std::string& amlFilePath, Log& log) :
		Importer(amlFilePath, log),
		amlFilePath(amlFilePath),
		pathToWorkingDirectory(amlFilePath.substr(0, amlFilePath.find_last_of('/') + 1)),
		massagers(),
		frameImporter()
	{
		return;
	}

	AMLImporter::~AMLImporter(){
		for (auto massagerEntries : massagers){
			delete(massagerEntries.second);
		}
		for (auto importerEntry : importers){
			delete(importerEntry);
		}
	}

	void AMLImporter::addElementsTo(Folder& root) {
		Poco::XML::AutoPtr<Poco::XML::Document> document = parseAMLFile();
		Poco::XML::NodeList* externalInterfaces = document->getElementsByTagName("ExternalInterface");
		findAndImportColladaReferences(externalInterfaces, root);
		convertZUpToYUp(root);
		std::cout << "AML / trans " << root.getBlobByType("transform")->getHash() << std::endl;
	}

	std::string AMLImporter::extractFilneNameFromURI(Poco::URI& refURI){
		std::vector<std::string> pathSegments;
		refURI.getPathSegments(pathSegments);
		return pathSegments.back();
	}

	Poco::XML::AutoPtr<Poco::XML::Document> AMLImporter::parseAMLFile(){
		std::ifstream amlStream(amlFilePath.getPath());
		Poco::XML::InputSource amlFileSource(amlStream);
		Poco::XML::DOMParser parser;
		Poco::XML::AutoPtr<Poco::XML::Document> doc = parser.parse(&amlFileSource);
		amlStream.close();
		return doc;
	}

	void AMLImporter::findAndImportColladaReferences(Poco::XML::NodeList* externalInterfaces, Folder& root) {
		for (int i = 0; i < externalInterfaces->length(); ++i) {
			Poco::XML::Node* node = externalInterfaces->item(i);
			Poco::URI refURI = extractRefURIOfExternalInterface(node);
			if (!uriReferencesColladaFile(refURI)){
				continue; // we do not have to handle external interfaces different from collada
			}
			std::string refType = extractRefTypeOfExternalInterface(node);
			if (refType == "explicit"){
				importColladaReference(refURI, node, root);
			}
		}
	}

	void AMLImporter::importColladaReference(Poco::URI& refURI, Poco::XML::Node* node, Folder& root) {
		fixColladaURIPath(refURI);
		//we need to add additional nodes in case of multiple references within the aml file and to handle the frame attributes
		Folder& colladaFolder = frameImporter.createParentHierarchy(node, root);
		colladaFolder.setName(extractFilneNameFromURI(refURI));
		importGeometryReference(colladaFolder, refURI);
	}

	std::string AMLImporter::extractRefTypeOfExternalInterface(Poco::XML::Node* externalInterface){
		Poco::XML::Node* refTypeValue = externalInterface->getNodeByPath("/Attribute[@Name='refType']/Value");
		if (refTypeValue == NULL) {
			//refType is a mandatory attribute, if its missing the AML is not valid and we should ignore this object.
			return "";
		}
		std::string refType = refTypeValue->innerText();
		std::transform(refType.begin(), refType.end(), refType.begin(), ::tolower);
		return refType;
	}

	Poco::URI AMLImporter::extractRefURIOfExternalInterface(Poco::XML::Node* externatlInterface){
		Poco::XML::Node* refNodeValue = externatlInterface->getNodeByPath("/Attribute[@Name='refURI']/Value");
		if (refNodeValue == NULL) {
			//This interface should be ignored, without a refURI it can't possibly be interesting for us
			return Poco::URI();
		}
		return Poco::URI(refNodeValue->innerText());
	}

	bool AMLImporter::uriReferencesColladaFile(Poco::URI refURI){
		std::string uriAsString = refURI.getPath();
		auto index = boost::algorithm::ifind_first(uriAsString, ".dae").begin() - uriAsString.begin();
		return index > 0;
	}

	void AMLImporter::fixColladaURIPath(Poco::URI& uri){
		std::string newPath = pathToWorkingDirectory.getPath();
		newPath.append(uri.getPath());
		uri.setPath(newPath);
	}

	aiMatrix4x4 AMLImporter::getTransformFor(Folder& folder) {
		ATLAS::Model::Blob* currentFolderTransform = folder.getBlobByType("transform");
		if (currentFolderTransform) {
			return *(aiMatrix4x4*)currentFolderTransform->getData();
		} else {
			aiMatrix4x4 idendity;
			return idendity;
		}
	}

	void AMLImporter::setTransformFor(Folder& folder, const aiMatrix4x4& newTransfrom) {
		DataDeletingBlob<aiMatrix4x4> blob(new aiMatrix4x4(newTransfrom));
		folder.addBlob("transform", blob);
	}

	void AMLImporter::convertZUpToYUp(Folder& folder) {
		aiMatrix4x4 transform = getTransformFor(folder);
		const aiMatrix4x4 zUpToYUp(
				1,  0,  0,  0,
				0,  0,  1,  0,
				0, -1,  0,  0,
				0,  0,  0,  1
		);
		transform *= zUpToYUp;
		setTransformFor(folder, transform);
	}

	void AMLImporter::convertYUpToZUp(Folder& folder) {
		aiMatrix4x4 transform = getTransformFor(folder);
		const aiMatrix4x4 yUpToZUp(
				1,  0,  0,  0,
				0,  0, -1,  0,
				0,  1,  0,  0,
				0,  0,  0,  1
		);
		transform *= yUpToZUp;
		setTransformFor(folder, transform);
	}

	void AMLImporter::importGeometryReference(Folder& root, const Poco::URI& colladaFileURI) {
		if (colladaFileURI.getFragment() == "") {
			importCompleteColladaScene(root, colladaFileURI);
		}
		else {
			importSubtreeOfColladaScene(root, colladaFileURI);
		}
		convertYUpToZUp(root);
	}

	void AMLImporter::importCompleteColladaScene(Folder& root, const Poco::URI& colladaFileURI){
		AssimpWorker::AssimpImporter* importer = new AssimpWorker::AssimpImporter();
		importers.push_back(importer);
		const aiScene* scene = importer->importSceneFromFile(colladaFileURI.getPath(), log);
		if (!scene) {
			return;
		}
		AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
		sceneImporter.addElementsTo(root);
	}

	void AMLImporter::importSubtreeOfColladaScene(Folder& root, const Poco::URI& colladaFileURI){
		ColladaMassager* massager = getMassagerForURI(colladaFileURI);
		AssimpWorker::AssimpImporter* importer = new AssimpWorker::AssimpImporter();
		importers.push_back(importer);
		const aiScene* scene = importer->importSceneFromFile(colladaFileURI.getPath(), log);
		if (!scene) {
			return;
		}
		aiNode* startingPoint = findaiNodeWithName(scene->mRootNode, colladaFileURI.getFragment());
		if (startingPoint == NULL){
			throw AMLException("Could not find a Node with id '" + colladaFileURI.getFragment() + "'");
		}
		restoreOriginalNames(startingPoint, massager);
		AiSceneImporter sceneImporter(scene, pathToWorkingDirectory.getPath(), log);
		sceneImporter.importSubtreeOfScene(root, startingPoint);
	}

	ColladaMassager* AMLImporter::getMassagerForURI(const Poco::URI& colladaFileURI) {
		std::map<std::string, ColladaMassager*>::iterator iter = massagers.find(colladaFileURI.getPath());
		if (iter != massagers.end()){
			return iter->second;
		}
		ColladaMassager* massager = new ColladaMassager(colladaFileURI);
		massagers.insert(std::make_pair(colladaFileURI.getPath(), massager));
		massager->purgeNames(); // Must be done here to avoid purging names twice for the same file
		return massager;
	}

	void AMLImporter::restoreOriginalNames(aiNode* node, ColladaMassager* massager) {
		node->mName = massager->getNameForId(node->mName.C_Str());
		for (int i = 0; i < node->mNumChildren; i++){
			restoreOriginalNames(node->mChildren[i], massager);
		}
	}

	aiNode* AMLImporter::findaiNodeWithName(aiNode* node, const std::string& name){
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

} // End namespace AssimpWorker
