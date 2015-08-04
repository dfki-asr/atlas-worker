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
		Poco::XML::NamedNodeMap* attributes = node->attributes();
		Poco::XML::Node* nameNode = attributes->getNamedItem("Name");
		if (nameNode == NULL) {
			nameNode = attributes->getNamedItem("name");
		}
		if (nameNode != NULL) {
			name = nameNode->getNodeValue();
		}
		attributes->release();
		return name;
	}

	ATLAS::Model::Folder& AMLFrameImporter::getFolderForNode(Poco::XML::Node* node, ATLAS::Model::Folder& parentFolder) {
		Poco::XML::Node* frame = node->getNodeByPath("/Attribute[@Name='Frame']");
		aiMatrix4x4 localTransform = createFrameTransformMatrix(frame);

		auto visited = visitedNodes.find(node);
		if (visited != visitedNodes.end()) {
			return *visited->second; //Already created a folder for this node so just return that one
		}
		else {
			ATLAS::Model::Folder& folderForNode = createFolderForNode(node, parentFolder);
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
		if (frame == NULL) {
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
			aiMatrix4x4 localTransform = buildRotationMatrix(rx, ry, rz);
			localTransform.a4 = (float)tx;
			localTransform.b4 = (float)ty;
			localTransform.c4 = (float)tz;
			return localTransform;
		}
		catch (const Exception& e) {
			throw AMLException("Syntax error while parsing a Frame attribute. This transformation will be ignored.");
		}
	}

	aiMatrix4x4 AMLFrameImporter::buildRotationMatrix(double attitude, double heading, double bank) {
		//Algorithm taken from http://www.euclideanspace.com/maths/geometry/rotations/conversions/eulerToMatrix/
		aiMatrix4x4 rot;
		const double toRad = M_PI / 180;
		const double sa = sin(attitude * toRad);
		const double ca = cos(attitude * toRad);
		const double sb = sin(bank * toRad);
		const double cb = cos(bank * toRad);
		const double sh = sin(heading * toRad);
		const double ch = cos(heading * toRad);
		rot.a1 = (float)(ch*ca);	rot.b1 = (float)(-ch*sa*cb + sh*sb);	rot.c1 = (float)(ch*sa*sb + sh*cb);
		rot.a2 = (float)(sa);		rot.b2 = (float)(ca*cb);				rot.c2 = (float)(-ca*sb);
		rot.a3 = (float)(-sh*ca);	rot.b3 = (float)(sh*sa*cb + ch*sb);		rot.c3 = (float)(-sh*sa*sb + ch*cb);
		return rot;
	}

}
