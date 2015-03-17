/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "../Importer.hpp"
#include "../AssimpImporter.hpp"
#include "ColladaMassager.hpp"
#include <atlas/model/Asset.hpp>
#include <assimp/scene.h>
#include <Poco/DOM/NodeList.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Document.h>
#include <Poco/URI.h>
#include "AMLFrameImporter.hpp"

namespace AssimpWorker {

	class AMLImporter : public Importer {
	public:
		AMLImporter(const std::string& amlFilePath, Log& log);
		~AMLImporter();
		virtual void addElementsTo(ATLAS::Model::Folder& asset);

	private:
		Poco::URI amlFilePath;
		Poco::URI pathToWorkingDirectory;
		std::map<std::string, ColladaMassager*> massagers;
		std::vector<AssimpWorker::AssimpImporter*> importers;
		AMLFrameImporter frameImporter;

		Poco::XML::AutoPtr<Poco::XML::Document> parseAMLFile();
		std::string extractFilneNameFromURI(Poco::URI& refURI);
		std::string extractRefTypeOfExternalInterface(Poco::XML::Node* externatlInterface);
		Poco::URI extractRefURIOfExternalInterface(Poco::XML::Node* externatlInterface);
		void findAndImportColladaReferences(Poco::XML::NodeList* externalInterfaces, ATLAS::Model::Folder& root);
		bool uriReferencesColladaFile(Poco::URI refURI);
		void fixColladaURIPath(Poco::URI& uri);

		void addGeometryToFolder(ATLAS::Model::Folder& asset, const Poco::URI& colladaFileURI, ColladaMassager& purger);
		void addFrameTransformToFolder(ATLAS::Model::Folder& folder, aiMatrix4x4* transform);
		void importColladaReference(Poco::URI& refURI, Poco::XML::Node* node, ATLAS::Model::Folder& root);
		void importCompleteColladaScene(ATLAS::Model::Folder& root, const Poco::URI& colladaFileURI);
		void importSubtreeOfColladaScene(ATLAS::Model::Folder& root, const Poco::URI& colladaFileURI);
		void importGeometryReference(ATLAS::Model::Folder& asset, const Poco::URI& colladaFileURI);
		ColladaMassager* getMassagerForURI(const Poco::URI& uri);
		aiNode* findaiNodeWithName(aiNode* node, const std::string& name);
		void restoreOriginalNames(aiNode* node, ColladaMassager* purger);
		aiMatrix4x4 getTransformFor(ATLAS::Model::Folder& folder);
		void setTransformFor(ATLAS::Model::Folder& folder, const aiMatrix4x4& newTransfrom);
		void convertZUpToYUp(ATLAS::Model::Folder& folder);
		void convertYUpToZUp(ATLAS::Model::Folder& folder);
	};

} // End namespace AssimpWorker

