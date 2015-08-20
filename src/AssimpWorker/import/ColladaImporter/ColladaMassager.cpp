/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <assimp/postprocess.h>
#include <fstream>
#include <sstream>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/Node.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/SAX/InputSource.h>
#pragma warning(push, 3)
	#include <Poco/FileStream.h>
	#include <Poco/File.h>
#pragma warning(pop)
#include "ColladaMassager.hpp"
#include "../../internal/Exception.hpp"

namespace AssimpWorker {

	static std::string FAKE_INTERNAL_REFERENCE = "#";

	ColladaMassager::ColladaMassager(const Poco::URI& uri) :
		idToNameMap(),
		parentIDToExternalURL(),
		uri(uri),
		xmlDocument(),
		idCounter(0),
		alreadyMassagedMyFile(false)
	{
		needToPurge = uri.getFragment() != "";
	}

	ColladaMassager::~ColladaMassager(){
		return;
	}

	void ColladaMassager::massage() {
		if (alreadyMassagedMyFile){
			return;
		}
		try
		{
			readXML();
			readUpAxis();
			forceUnitMeter();
			extractExternalReferences();
			purgeAllNodes();
			writePurgedXML();
			alreadyMassagedMyFile = true;
		}
		catch (const std::exception& e) {
			std::cout << "Exception while trying to purge names from Collada file: " << e.what() << std::endl;
		}
	}

	void ColladaMassager::readXML(){
		std::ifstream colladaStream(uri.getPath());
		Poco::XML::InputSource colladaFileSource(colladaStream);
		Poco::XML::DOMParser parser;
		parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, false);
		xmlDocument = parser.parse(&colladaFileSource);
		colladaStream.close();
	}

	void ColladaMassager::readUpAxis(){
		Poco::AutoPtr<Poco::XML::NodeList> nodes = xmlDocument->getElementsByTagName("up_axis");
		for (int i = 0; i < nodes->length(); i++) {
			Poco::XML::Node* upAxisNode = nodes->item(i);
			upAxis = upAxisNode->innerText();
		}
	}

	const std::string ColladaMassager::getUpAxis(){
		return upAxis;
	}

	void ColladaMassager::extractExternalReferences(){
		Poco::AutoPtr<Poco::XML::NodeList> nodes = xmlDocument->getElementsByTagName("instance_node");
		for (int i = 0; i < nodes->length(); ++i) {
			Poco::XML::Node* node = nodes->item(i);
			std::string url = getURLOfReference(node);
			if (isInternalReference(url)){
				continue;
			}
			const std::string id = getIDOfParentNode(node);
			parentIDToExternalURL.push_back(std::make_pair(id, url));
			Poco::AutoPtr<Poco::XML::NamedNodeMap> attributesMap = node->attributes();
			attributesMap->removeNamedItem("url");
		}
	}

	bool ColladaMassager::isInternalReference(const std::string& url) {
		return url.at(0) == '#';
	}

	const std::string& ColladaMassager::getURLOfReference(Poco::XML::Node* node) {
		Poco::AutoPtr<Poco::XML::NamedNodeMap> attributesMap = node->attributes();
		Poco::XML::Node* urlNode = attributesMap->getNamedItem("url");
		if (urlNode == nullptr){
			return FAKE_INTERNAL_REFERENCE;
		}
		return urlNode->getNodeValue();
	}

	const std::string ColladaMassager::getIDOfParentNode(Poco::XML::Node* node){
		Poco::XML::Node* parent = node->parentNode();
		Poco::AutoPtr<Poco::XML::NamedNodeMap> attributesMap = parent->attributes();
		Poco::XML::Node* parentIdNode = attributesMap->getNamedItem("id");
		std::string id;
		if (parentIdNode != nullptr){
			id = parentIdNode->getNodeValue();
		}
		else {
			std::ostringstream generatedIdString;
			generatedIdString << "generated_" << idCounter;
			idCounter++;
			id = generatedIdString.str();
			Poco::XML::Node* idNode = xmlDocument->createElement("id");
			idNode->setNodeValue(id);
			attributesMap->setNamedItem(idNode);
			idNode->release();
		}
		return id;
	}

	const float ColladaMassager::getCurrentUnit() const {
		return localScaleBeforeMassage;
	}

	std::vector<std::pair<std::string, std::string>>& ColladaMassager::getExternalReferences(){
		return parentIDToExternalURL;
	}

	void ColladaMassager::purgeAllNodes(){
		Poco::AutoPtr<Poco::XML::NodeList> nodes = xmlDocument->getElementsByTagName("node");
		for (int i = 0; i < nodes->length(); ++i) {
			purgeNode(nodes->item(i));
		}
	}

	void ColladaMassager::forceUnitMeter() {
		Poco::AutoPtr<Poco::XML::NodeList> unitNodes = xmlDocument->getElementsByTagName("unit");
		for (int i = 0; i < unitNodes->length(); i++) {
			Poco::AutoPtr<Poco::XML::NamedNodeMap> attributesMap = unitNodes->item(i)->attributes();
			Poco::XML::Node* meterNode = attributesMap->getNamedItem("meter");
			Poco::XML::Node* nameNode = attributesMap->getNamedItem("name");
			localScaleBeforeMassage = atof((meterNode->getNodeValue()).c_str());
			meterNode->setNodeValue("1");
			nameNode->setNodeValue("m");
		}
	}

	void ColladaMassager::purgeNode(Poco::XML::Node* node){
		Poco::AutoPtr<Poco::XML::NamedNodeMap> attributesMap = node->attributes();
		Poco::XML::Node* nameNode = attributesMap->getNamedItem("name");
		Poco::XML::Node* idNode = attributesMap->getNamedItem("id");
		if (nameNode == nullptr || idNode == nullptr) {
			return;
		}
		std::string name = nameNode->getNodeValue();
		std::string id = idNode->getNodeValue();
		idToNameMap.insert(std::make_pair(id, name));
		attributesMap->removeNamedItem("name");
	}

	void ColladaMassager::writePurgedXML(){
		//we remove something from the content and write into the same file,
		//leading to some leftover garbage at the end of the file
		//to remove the garbage: remove and recreate the file
		Poco::File file(uri.getPath());
		file.remove();
		file.createFile();
		Poco::FileStream ofs(uri.getPath(), std::ios::in);
		Poco::XML::DOMWriter writer;
		writer.writeNode(ofs, xmlDocument);
		ofs.flush();
		ofs.close();
	}

	std::string ColladaMassager::getNameForId(const std::string& id) {
		auto it = idToNameMap.find(id);
		if (it == idToNameMap.end()){
			throw Exception("Did not find id in map: " + id);
		}
		return it->second;
	}

	void ColladaMassager::restoreOriginalNames(aiNode* node) {
		node->mName = getNameForId(node->mName.C_Str());
		for (int i = 0; i < node->mNumChildren; i++){
			restoreOriginalNames(node->mChildren[i]);
		}
	}

	void ColladaMassager::restoreOriginalNames(ATLAS::Model::Folder& folder) {
		const std::string colladaID = folder.getName();
		std::string colladaName;
		try{
			colladaName = getNameForId(colladaID);
		}catch (Exception ex){
			colladaName = colladaID;
		}
		folder.addAttribute("colladaID", colladaID);
		folder.setName(colladaName);
		for (ATLAS::Model::Folder& child : folder.getChildren()){
			restoreOriginalNames(child);
		}
	}
}
