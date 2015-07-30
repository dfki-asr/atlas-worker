/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <assimp/postprocess.h>
#include <fstream>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/Node.h>
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/SAX/InputSource.h>
#pragma warning(push, 3)
	#include <Poco/FileStream.h>
#pragma warning(pop)
#include "ColladaMassager.hpp"

namespace AssimpWorker {

	ColladaMassager::ColladaMassager(const Poco::URI& uri) :
		idToNameMap(),
		parentIDToExternalURL(),
		uri(uri),
		xmlDocument(),
		idCounter(0)
	{
		needToPurge = uri.getFragment() != "";
	}

	ColladaMassager::~ColladaMassager(){
		return;
	}

	void ColladaMassager::massage() {
		try
		{
			readXML();
			handleExternalReferences();
			if (needToPurge){
				purgeAllNodes();
				writePurgedXML();
			}
		}
		catch (const std::exception& e) {
			std::cout << "Exception while trying to purge names from Collada file: Error #" << e.what() << std::endl;
		}
	}

	void ColladaMassager::readXML(){
		std::ifstream amlStream(uri.getPath());
		Poco::XML::InputSource amlFileSource(amlStream);
		Poco::XML::DOMParser parser;
		parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, false);
		xmlDocument = parser.parse(&amlFileSource);
		amlStream.close();
	}

	void ColladaMassager::handleExternalReferences(){
		Poco::XML::NodeList* nodes = xmlDocument->getElementsByTagName("instance_node");
		for (int i = 0; i < nodes->length(); ++i) {
			Poco::XML::Node* node = nodes->item(i);
			Poco::XML::Node* urlNode = node->attributes()->getNamedItem("url");
			if (urlNode == NULL){
				continue;
			}
			std::string url = urlNode->getNodeValue();
			if (url.at(0) == '#'){
				continue;
			}
			Poco::XML::Node* parent = node->parentNode();
			Poco::XML::Node* parentIdNode = parent->attributes()->getNamedItem("id");
			std::string id;
			if (parentIdNode != NULL){
				id = parentIdNode->getNodeValue();
			} else {
				id = "generated_" + idCounter;
				idCounter++;
				Poco::XML::Node* idNode = xmlDocument->createElement("id");
				idNode->setNodeValue(id);
				parent->attributes()->setNamedItem(idNode);
			}
			std::cout << "Removed node with id: " << id << "and URL: " << url << std::endl;
			parentIDToExternalURL.insert(std::make_pair(id, url));
			parent->removeChild(node);
		}
	}

	std::map<std::string, std::string>& ColladaMassager::getExternalReferences(){
		return parentIDToExternalURL;
	}

	void ColladaMassager::purgeAllNodes(){
		Poco::XML::NodeList* nodes = xmlDocument->getElementsByTagName("node");
		for (int i = 0; i < nodes->length(); ++i) {
			purgeNode(nodes->item(i));
		}
	}

	void ColladaMassager::purgeNode(Poco::XML::Node* node){
		Poco::XML::Node* nameNode = node->attributes()->getNamedItem("name");
		Poco::XML::Node* idNode = node->attributes()->getNamedItem("id");
		if (nameNode == NULL || idNode == NULL) {
			return;
		}
		std::string name = nameNode->getNodeValue();
		std::string id = idNode->getNodeValue();
		idToNameMap.insert(std::make_pair(id, name));
		node->attributes()->removeNamedItem("name");
	}

	void ColladaMassager::writePurgedXML(){
		Poco::FileStream ofs(uri.getPath(), std::ios::in);
		Poco::XML::DOMWriter writer;
		writer.writeNode(ofs, xmlDocument);
		ofs.close();
	}

	std::string ColladaMassager::getNameForId(const std::string& id) {
		return idToNameMap.find(id)->second;
	}

	void ColladaMassager::restoreOriginalNames(aiNode* node) {
		if (!needToPurge){
			return;
		}
		node->mName = getNameForId(node->mName.C_Str());
		for (int i = 0; i < node->mNumChildren; i++){
			restoreOriginalNames(node->mChildren[i]);
		}
	}
}
