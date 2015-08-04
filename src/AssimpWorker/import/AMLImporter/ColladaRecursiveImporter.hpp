/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "ColladaMassager.hpp"
#include "ColladaMassagerRegistry.hpp"
#include "../AssimpImporter.hpp"
#include "../Importer.hpp"
#include <assimp/scene.h>
#include <atlas/model/Folder.hpp>

namespace AssimpWorker {

	class ColladaRecursiveImporter :public Importer
	{
	public:
		ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, Poco::URI pathToWorkingDirectory, ColladaMassagerRegistry& registry);

		~ColladaRecursiveImporter();

		virtual void addElementsTo(ATLAS::Model::Folder& asset);
		
	private:
		AssimpImporter* importer;
		std::vector<ColladaRecursiveImporter*> childImporter;
		Poco::URI pathToWorkingDirectory;
		const Poco::URI& colladaFileURI;
		ColladaMassagerRegistry& massagerRegistry;

		std::string fixRelativeReference(std::string relativeURI);
		ATLAS::Model::Folder& findFolderWithName(ATLAS::Model::Folder& root, std::string name);
		aiNode* findaiNodeWithName(aiNode* node, const std::string& name);
		ATLAS::Model::Folder& findFolderWithColladaID(ATLAS::Model::Folder& folder, std::string id);
		void restoreOriginalNames(aiNode* node);
	};

} // End namespace AssimpWorker

