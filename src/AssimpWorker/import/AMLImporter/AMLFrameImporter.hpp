/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <atlas/model/Blob.hpp>
#include <atlas/model/Folder.hpp>
#include <Poco/DOM/Node.h>
#include <stack>
#include <assimp/scene.h>

namespace AssimpWorker {

	class AMLFrameImporter {
	public:
		AMLFrameImporter();

		ATLAS::Model::Folder& createParentHierarchy(Poco::XML::Node* node, ATLAS::Model::Folder& root);
		static void addTransformBlobToFolder(const aiMatrix4x4& transform, ATLAS::Model::Folder& folder, const std::string& transformType);

	private:
		std::map<Poco::XML::Node*, ATLAS::Model::Folder*> visitedNodes;

		void buildHierarchyStack(const Poco::XML::Node* node, std::stack<Poco::XML::Node*>& parentNodes);
		ATLAS::Model::Folder& createFolderStructureForStack(std::stack<Poco::XML::Node*>& parentNodeStack, ATLAS::Model::Folder& root);
		ATLAS::Model::Folder& getFolderForNode(Poco::XML::Node* node, ATLAS::Model::Folder& folder);
		ATLAS::Model::Folder& createFolderForNode(Poco::XML::Node* node, ATLAS::Model::Folder& folder);
		aiMatrix4x4 createFrameTransformMatrix(const Poco::XML::Node* frame);
		std::string getNameAttributeForNode(Poco::XML::Node* node);
	};

}
