/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <fstream>
#include <assimp/scene.h>
#include "AiMaterialImporter.hpp"
#include "../FileImporter.hpp"
#include "../../internal/Exception.hpp"
#include <atlas/model/AtlasMaterial.hpp>
#include <atlas/model/Blob.hpp>

namespace AssimpWorker {

	using ATLAS::Model::Folder;
	using ATLAS::Model::DataDeletingBlob;
	using ATLAS::Model::DataFreeingBlob;
	using ATLAS::Model::Material;

	/////////////////////////// AiMaterialImporter
	const aiTextureType AiMaterialImporter::supportedTexTypes[11] = { aiTextureType_AMBIENT, aiTextureType_DIFFUSE, aiTextureType_DISPLACEMENT, aiTextureType_EMISSIVE,
			aiTextureType_HEIGHT, aiTextureType_LIGHTMAP, aiTextureType_NORMALS, aiTextureType_OPACITY, aiTextureType_REFLECTION,
			aiTextureType_SHININESS, aiTextureType_SPECULAR };
	const unsigned int AiMaterialImporter::numSupportedTextureTypes = 11;

	AiMaterialImporter::AiMaterialImporter(const std::string& pathToFolder, const aiMaterial* material, Log& log) :
		Importer(pathToFolder, log),
		material(material),
		pathToFolder(pathToFolder)
	{
		return;
	}

	AiMaterialImporter::~AiMaterialImporter(){
		return; 
	}
	void AiMaterialImporter::addElementsTo(Folder& parent){
		Folder& materialFolder = parent.appendChild("material");
		aiString name;
		aiShadingMode shadingMode;
		material->Get(AI_MATKEY_NAME, name);
		if(material->Get(AI_MATKEY_SHADING_MODEL, shadingMode) != AI_SUCCESS) {
			// "If absent, assume phong shading only if a specular exponent is given."
			// -- http://assimp.sourceforge.net/lib_html/materials.html
			shadingMode = aiShadingMode_Phong;
		}
		materialFolder.setName(name.C_Str());
		materialFolder.addAttribute("shadingModel", getShadingModelString(shadingMode));
		addMaterialBlob(materialFolder);
		addTextureFolders(materialFolder);
	}

	void AiMaterialImporter::addMaterialBlob(Folder& materialFolder) {
		Material* mat = new Material;
		copyColor(AI_MATKEY_COLOR_AMBIENT, mat->ambient);
		copyColor(AI_MATKEY_COLOR_DIFFUSE, mat->diffuse);
		copyColor(AI_MATKEY_COLOR_EMISSIVE, mat->emissive);
		copyColor(AI_MATKEY_COLOR_SPECULAR, mat->specular);
		material->Get(AI_MATKEY_OPACITY, mat->opacity);
		material->Get(AI_MATKEY_SHININESS, mat->shininess);

		DataDeletingBlob<Material> materialBlob(mat);
		materialFolder.addBlob("material", materialBlob);
	}

	void AiMaterialImporter::copyColor(const char* key, unsigned int type, unsigned int idx, ATLAS::Model::Color& color) {
		aiColor3D tempColor;
		material->Get(key, type, idx, tempColor);
		color.r = tempColor.r;
		color.g = tempColor.g;
		color.b = tempColor.b;
	}

	void AiMaterialImporter::addTextureFolders(Folder& materialFolder) {
		for (unsigned int i = 0; i < numSupportedTextureTypes; i++) {
			if (material->GetTextureCount(supportedTexTypes[i]) > 0) {
				aiString texturePath;
				material->Get(AI_MATKEY_TEXTURE(supportedTexTypes[i], 0), texturePath);
				addTextureFolder(texturePath, materialFolder, supportedTexTypes[i]);
			}
			if (material->GetTextureCount(supportedTexTypes[i]) > 1) {
				aiString name;
				material->Get(AI_MATKEY_NAME, name);
				log.info("Currently only one texture per channel is supported. Only the first texture in channel " +
					getTextureTypeString(supportedTexTypes[i]) + " for material " + name.C_Str() + " will be kept.");
			}
		}
	}

	void AiMaterialImporter::addTextureFolder(aiString& texturePath, Folder& materialFolder, aiTextureType type) {
		Folder& textureFolder = materialFolder.appendChild(getTextureTypeString(type));
		textureFolder.addAttribute("filename", getFilenameFromTexturePath(texturePath));

		std::string completePath = pathToFolder + std::string(texturePath.C_Str());
		FileImporter textureFileImporter(completePath, log);
		textureFileImporter.setFileType(getTextureTypeString(type));
		textureFileImporter.addElementsTo(textureFolder);
	}

	std::string AiMaterialImporter::getFilenameFromTexturePath(aiString& texturePath) {
		std::string path = std::string(texturePath.C_Str());
		auto fileSuffixIndex = path.find_last_of("/");
		return std::string(path.substr(fileSuffixIndex + 1));
	}

	std::string AiMaterialImporter::getTextureTypeString(aiTextureType type) {
		switch (type) {
		case aiTextureType_AMBIENT: return "ambient";
		case aiTextureType_DIFFUSE: return "diffuse";
		case aiTextureType_DISPLACEMENT: return "displacement";
		case aiTextureType_EMISSIVE: return "emissive";
		case aiTextureType_HEIGHT: return "height";
		case aiTextureType_LIGHTMAP: return "lightmap";
		case aiTextureType_NORMALS: return "normals";
		case aiTextureType_OPACITY: return "opacity";
		case aiTextureType_REFLECTION: return "reflection";
		case aiTextureType_SHININESS: return "shininess";
		case aiTextureType_SPECULAR: return "specular";
		default: return "UNKNOWN";
		}
	}

	std::string AiMaterialImporter::getShadingModelString(aiShadingMode shadingMode) {
		switch (shadingMode) {
		case aiShadingMode_Blinn: return "blinn";
		case aiShadingMode_CookTorrance: return "cookTorrance";
		case aiShadingMode_Flat: return "flat";
		case aiShadingMode_Fresnel: return "fresnel";
		case aiShadingMode_Gouraud: return "gouraud";
		case aiShadingMode_Minnaert: return "minnaert";
		case aiShadingMode_NoShading: return "noShading";
		case aiShadingMode_OrenNayar: return "orenNayar";
		case aiShadingMode_Phong: return "phong";
		case aiShadingMode_Toon: return "toon";
		default: return "phong";
		}
	}

} // End namespace AssimpWorker
