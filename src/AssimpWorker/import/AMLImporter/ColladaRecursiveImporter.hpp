/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "ColladaMassager.hpp"
#include "../AssimpImporter.hpp"
#include "../Importer.hpp"
#include <assimp/scene.h>

namespace AssimpWorker {

	class ColladaRecursiveImporter :public Importer
	{
	public:
		ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, Poco::URI pathToWorkingDirectory);

		~ColladaRecursiveImporter();

		virtual void addElementsTo(ATLAS::Model::Folder& asset);
		
	private:
		ColladaMassager massager;
		AssimpImporter* importer;
		Poco::URI pathToWorkingDirectory;
		const Poco::URI& colladaFileURI;

		aiNode* findaiNodeWithName(aiNode* node, const std::string& name);
		void restoreOriginalNames(aiNode* node);
	};

} // End namespace AssimpWorker


