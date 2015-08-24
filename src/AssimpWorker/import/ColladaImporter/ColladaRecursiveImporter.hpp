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
#include "../AiImporter/AiSceneImporter.hpp"

namespace AssimpWorker {

	class ColladaRecursiveImporter :public Importer
	{
	public:
		/** Constructor for "Root" Collada File to recursively import. */
		ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, ColladaMassagerRegistry& registry);
		~ColladaRecursiveImporter();

		virtual void addElementsTo(ATLAS::Model::Folder& asset);
		const float getLocalScale() const;
		const std::string getColladaUpAxis();
		
	private:
		/** Internal constructor for Child documents. */
		ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, const ColladaRecursiveImporter& parent);

		AssimpImporter* importer;
		std::vector<ColladaRecursiveImporter*> childImporter;
		const Poco::URI& colladaFileURI;
		ColladaMassagerRegistry& massagerRegistry;
		ColladaMassager* massager;
		const float parentScale;

		void preprocessCollada();
		const aiScene* runAssimpImport();
		void convertToFolderStructure(const aiScene* scene, ATLAS::Model::Folder& root);
		void importChildColladas(ATLAS::Model::Folder& root);

		std::string extractDirectory(const Poco::URI& fileURI);
		std::string fixRelativeReference(const std::string relativeURI);
		ATLAS::Model::Folder* findFolderWithName(ATLAS::Model::Folder& root, const std::string& name);
		ATLAS::Model::Folder* findFolderWithColladaID(ATLAS::Model::Folder& folder, const std::string& id);
	};

} // End namespace AssimpWorker


