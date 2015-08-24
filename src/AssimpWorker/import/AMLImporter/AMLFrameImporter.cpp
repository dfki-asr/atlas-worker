/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#define _USE_MATH_DEFINES

#include "AMLFrameImporter.hpp"
#include "../../internal/Exception.hpp"
#include <Poco/DOM/NamedNodeMap.h>
#include <Poco/DOM/AutoPtr.h>

namespace AssimpWorker{

	using ATLAS::Model::DataDeletingBlob;

	AMLFrameImporter::AMLFrameImporter(){
		return;
	}

	ATLAS::Model::Folder& AMLFrameImporter::createParentHierarchy(Poco::XML::Node* node, ATLAS::Model::Folder& root){
		std::stack<Poco::XML::Node*> parentNodeStack;
		buildHierarchyStack(node, parentNodeStack);
		return createFolderStructureForStack(parentNodeStack, root);
	}

	void AMLFrameImporter::buildHierarchyStack(const Poco::XML::Node* node, std::stack<Poco::XML::Node*>& parentNodes) {
		Poco::XML::Node* transformationRoot((Poco::XML::Node*) node);
		while (transformationRoot->localName() == "InternalElement" || transformationRoot->localName() == "ExternalInterface") {
			// Walk our way up the InternalElement hierarchy and add each node to a stack. Later we use the stack to 
			// walk back down the tree to build the transformation matrices and folder structure
			parentNodes.push(transformationRoot);
			transformationRoot = transformationRoot->parentNode();
		}
	}

	ATLAS::Model::Folder& AMLFrameImporter::createFolderStructureForStack(std::stack<Poco::XML::Node*>& parentNodeStack, ATLAS::Model::Folder& root) {
		Poco::XML::Node* currentNode = parentNodeStack.top();
		ATLAS::Model::Folder& currentFolder = getFolderForNode(currentNode, root);
		parentNodeStack.pop();
		if (parentNodeStack.empty()) {
			return currentFolder;
		}
		else {
			return createFolderStructureForStack(parentNodeStack, currentFolder);
		}
	}

	std::string AMLFrameImporter::getNameAttributeForNode(Poco::XML::Node* node) {
		std::string name = node->nodeName();
		Poco::XML::AutoPtr<Poco::XML::NamedNodeMap> attributes = node->attributes();
		Poco::XML::Node* nameNode = attributes->getNamedItem("Name");
		if (nameNode == nullptr) {
			nameNode = attributes->getNamedItem("name");
		}
		if (nameNode != nullptr) {
			name = nameNode->getNodeValue();
		}
		attributes->release();
		return name;
	}

	ATLAS::Model::Folder& AMLFrameImporter::getFolderForNode(Poco::XML::Node* node, ATLAS::Model::Folder& parentFolder) {
		auto visited = visitedNodes.find(node);
		if (visited != visitedNodes.end()) {
			return *visited->second; //Already created a folder for this node so just return that one
		}
		else {
			ATLAS::Model::Folder& folderForNode = createFolderForNode(node, parentFolder);
			Poco::XML::Node* frame = node->getNodeByPath("/Attribute[@Name='Frame']");
			aiMatrix4x4 localTransform = createFrameTransformMatrix(frame);
			addTransformBlobToFolder(localTransform, folderForNode, "transform");
			return folderForNode;
		}
	}

	ATLAS::Model::Folder& AMLFrameImporter::createFolderForNode(Poco::XML::Node* node, ATLAS::Model::Folder& parentFolder) {
		ATLAS::Model::Folder& nodeFolder = parentFolder.appendChild("node");
		nodeFolder.setName(getNameAttributeForNode(node));
		std::pair<Poco::XML::Node*, ATLAS::Model::Folder*> toInsert = std::make_pair(node, &nodeFolder);
		visitedNodes.insert(toInsert);
		return nodeFolder;
	}

	void AMLFrameImporter::addTransformBlobToFolder(const aiMatrix4x4& transform, ATLAS::Model::Folder& folder, const std::string& transformType) {
		aiMatrix4x4* transformCopy = new aiMatrix4x4(transform);
		DataDeletingBlob<aiMatrix4x4> transformBlob(transformCopy);
		folder.addBlob(transformType, transformBlob);
	}

	aiMatrix4x4 AMLFrameImporter::createFrameTransformMatrix(const Poco::XML::Node* frame) {
		if (frame == nullptr) {
			aiMatrix4x4 identity;
			return identity;
		}
		try {
			double tx = ::atof(frame->getNodeByPath("/Attribute[@Name='x']/Value")->innerText().c_str());
			double ty = ::atof(frame->getNodeByPath("/Attribute[@Name='y']/Value")->innerText().c_str());
			double tz = ::atof(frame->getNodeByPath("/Attribute[@Name='z']/Value")->innerText().c_str());
			double rx = ::atof(frame->getNodeByPath("/Attribute[@Name='rx']/Value")->innerText().c_str());
			double ry = ::atof(frame->getNodeByPath("/Attribute[@Name='ry']/Value")->innerText().c_str());
			double rz = ::atof(frame->getNodeByPath("/Attribute[@Name='rz']/Value")->innerText().c_str());
			const double toRad = M_PI / 180;
			aiMatrix4x4 temp;
			aiMatrix4x4 localTransform;
			// re-assemble Frame transform (which is in right-hand z-up) into a right-hand y-up transform
			// hence the shuffling of parameters and negative factors. mind the order of transforms!
			localTransform *= aiMatrix4x4::Translation(aiVector3D(tx,tz,-ty),temp);
			localTransform *= aiMatrix4x4::RotationX(rx*toRad, temp);
			localTransform *= aiMatrix4x4::RotationY(rz*toRad, temp);
			localTransform *= aiMatrix4x4::RotationZ(-ry*toRad, temp);
			return localTransform;
		}
		catch (const Exception& e) {
			throw AMLException("Syntax error while parsing a Frame attribute. This transformation will be ignored.");
		}
	}
}
