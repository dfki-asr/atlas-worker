/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#define _USE_MATH_DEFINES

#include "AMLImporter.hpp"
#include "../../internal/Exception.hpp"
#include "../ColladaImporter/ColladaMassagerRegistry.hpp"
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
		frameImporter(),
		colladaImporters(),
		massagerRegistry()
	{
		return;
	}

	AMLImporter::~AMLImporter(){
		for (auto i : colladaImporters){
			delete i;
		}
	}

	void AMLImporter::addElementsTo(Folder& root) {
		Poco::XML::AutoPtr<Poco::XML::Document> document = parseAMLFile();
		Poco::XML::AutoPtr<Poco::XML::NodeList> externalInterfaces = document->getElementsByTagName("ExternalInterface");
		findAndImportColladaReferences(externalInterfaces, root);
		removeColladaIDs(root);
	}

	void AMLImporter::removeColladaIDs(Folder& folder){
		folder.removeAttribute("colladaID");
		for (Folder& child : folder.getChildren()){
			removeColladaIDs(child);
		}
	}

	std::string AMLImporter::extractFileNameFromURI(Poco::URI& refURI){
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

	void AMLImporter::findAndImportColladaReferences(Poco::XML::AutoPtr<Poco::XML::NodeList> externalInterfaces, Folder& root) {
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
		prependAMLPath(refURI);
		Folder& colladaFolder = frameImporter.createParentHierarchy(node, root);
		colladaFolder.setName(extractFileNameFromURI(refURI));
		importGeometryReference(colladaFolder, refURI);
	}

	std::string AMLImporter::extractRefTypeOfExternalInterface(Poco::XML::Node* externalInterface){
		Poco::XML::Node* refTypeValue = externalInterface->getNodeByPath("/Attribute[@Name='refType']/Value");
		if (refTypeValue == nullptr) {
			//refType is a mandatory attribute, if its missing the AML is not valid and we should ignore this object.
			return "";
		}
		std::string refType = refTypeValue->innerText();
		std::transform(refType.begin(), refType.end(), refType.begin(), ::tolower);
		return refType;
	}

	Poco::URI AMLImporter::extractRefURIOfExternalInterface(Poco::XML::Node* externalInterface){
		Poco::XML::Node* refNodeValue = externalInterface->getNodeByPath("/Attribute[@Name='refURI']/Value");
		if (refNodeValue == nullptr) {
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

	void AMLImporter::prependAMLPath(Poco::URI& uri){
		std::string newPath = pathToWorkingDirectory.getPath();
		newPath.append(uri.getPath());
		uri.setPath(newPath);
	}

	void AMLImporter::importGeometryReference(Folder& root, const Poco::URI& colladaFileURI) {
		ColladaRecursiveImporter* importer = new ColladaRecursiveImporter(colladaFileURI, log, massagerRegistry);
		colladaImporters.push_back(importer);
		if (colladaFileURI.getFragment() != "") {
			// a fragment reference is imported ignoring up-vector and scale of the collada.
			// Thus, an additional intermediate node is needed to store the transform to
			// y-up, 1 unit = 1 meter
			Folder& upAndScale = root.appendChild("node");
			upAndScale.setName("normalizeCoordinates");
			importer->addElementsTo(upAndScale);
			// but we can only compute the normalization once the import is done:
			aiMatrix4x4 normalize = createNormalizationTransform(*importer);
			AMLFrameImporter::addTransformBlobToFolder(normalize, upAndScale, "transform");
		} else {
			// whole-file references have their transform to y-up and meters taken care of
			// by the ColladaRecursiveImporter.
			importer->addElementsTo(root);
		}
	}

	aiMatrix4x4 AMLImporter::createNormalizationTransform(const ColladaRecursiveImporter& importer)
	{
		aiMatrix4x4 transform;
		float scale = importer.getLocalScale();
		transform *= aiMatrix4x4(scale, 0, 0, 0,
		                         0, scale, 0, 0,
		                         0, 0, scale, 0,
		                         0, 0, 0, 1);
		if (importer.getColladaUpAxis() == "Z_UP") {
			transform *= aiMatrix4x4(
			                    1,  0,  0,  0,
			                    0,  0,  1,  0,
			                    0, -1,  0,  0,
			                    0,  0,  0,  1);
		} else if (importer.getColladaUpAxis() == "X_UP") {
			transform *= aiMatrix4x4(
			                    0, -1,  0,  0,
			                    1,  0,  0,  0,
			                    0,  0,  1,  0,
			                    0,  0,  0,  1);
		}
		// (nothing to do for Y_UP)
		return transform;
	}

} // End namespace AssimpWorker
