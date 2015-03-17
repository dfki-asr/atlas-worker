/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <atlas/model/AtlasMaterial.hpp>
#include "assimp/scene.h"
#include "../Importer.hpp"

namespace AssimpWorker {

	class AiMaterialImporter : public Importer {
	public:
		AiMaterialImporter(const std::string& pathToFolder, const aiMaterial* material, Log& log);
		virtual ~AiMaterialImporter();
		virtual void addElementsTo(ATLAS::Model::Folder& root);
	private:
		static const aiTextureType supportedTexTypes[11];
		static const unsigned int numSupportedTextureTypes;
		const aiMaterial* material;
		const std::string& pathToFolder;

		void addMaterialBlob(ATLAS::Model::Folder& folder);
		void addTextureFolders(ATLAS::Model::Folder& folder);
		void addTextureFolder(aiString& texturePath, ATLAS::Model::Folder& folder, aiTextureType type);
		void copyColor(const char* key, unsigned int type, unsigned int idx, ATLAS::Model::Color& color);
		std::string getFilenameFromTexturePath(aiString& texturePath);
		std::string getTextureTypeString(aiTextureType type);
		std::string getShadingModelString(aiShadingMode shadingMode);
	};

} // End namespace AssimpWorker

