/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <fstream>
#include <boost/iterator/filter_iterator.hpp>
#include "AiMeshImporter.hpp"
#include "AiMaterialImporter.hpp"
#include "../../internal/Exception.hpp"
#include <atlas/model/Blob.hpp>
#include "../../internal/configuration.hpp"

namespace AssimpWorker {

	using ATLAS::Model::DataUntouchingBlob;
	using ATLAS::Model::DataFreeingBlob;
	using ATLAS::Model::Folder;

	/////////////////////////// AiMeshImporter

	AiMeshImporter::AiMeshImporter(const aiScene* scene, const std::string& pathToFolder, const aiMesh* mesh, Log& log) :
		Importer(pathToFolder, log),
		mesh(mesh),
		scene(scene),
		pathToFolder(pathToFolder)
	{
		return;
	}

	AiMeshImporter::~AiMeshImporter(){
		return;
	}

	void AiMeshImporter::addElementsTo(Folder& parent){
		Folder& meshFolder = parent.appendChild("mesh");
		meshFolder.setName(parent.getName() + "-shape");
		meshFolder.addAttribute("primitiveType", getPrimitiveType());

		addFacesTo(meshFolder);
		addPositionsTo(meshFolder);
		addNormalsTo(meshFolder);
		addTexcoordsTo(meshFolder);
		addMaterialTo(meshFolder);
	}

	void AiMeshImporter::addPositionsTo(Folder& meshFolder) {
		DataUntouchingBlob positionBlob(mesh->mVertices, mesh->mNumVertices * sizeof(aiVector3D));
		meshFolder.addBlob("positions", positionBlob);
	}

	void AiMeshImporter::addTexcoordsTo(Folder& meshFolder) {
		//TODO: Expand support to include multiple texture coordinate sets?
		if (mesh->HasTextureCoords(0)) {
			DataUntouchingBlob texcoordBlob(mesh->mTextureCoords[0], mesh->mNumVertices * sizeof(aiVector3D));
			meshFolder.addBlob("texcoords", texcoordBlob);
		}
	}

	void AiMeshImporter::addNormalsTo(Folder& meshFolder) {
		if (mesh->HasNormals()) {
			DataUntouchingBlob normalBlob(mesh->mNormals, mesh->mNumVertices * sizeof(aiVector3D));
			meshFolder.addBlob("normals", normalBlob);
		}
	}

	void AiMeshImporter::addFacesTo(Folder& meshFolder) {
		int numFaces = mesh->mNumFaces;
		unsigned int* indices = (unsigned int*) malloc(numFaces * 3 * sizeof(unsigned int)); //TODO: Currently hardcoded to 3 vertices per face since non-triangle faces are dropped by the import options
		for (int i = 0; i < numFaces; ++i) {
			aiFace* face = &mesh->mFaces[i];
			std::copy(face->mIndices, face->mIndices + 3, indices + i * 3);
		}
		DataFreeingBlob indexBlob(indices, numFaces * 3 * sizeof(unsigned int));
		meshFolder.addBlob("index", indexBlob);
	}

	void AiMeshImporter::addMaterialTo(Folder& meshFolder) {
		AiMaterialImporter materialImporter(pathToFolder, scene->mMaterials[mesh->mMaterialIndex], log);
		materialImporter.addElementsTo(meshFolder);
	}

	std::string AiMeshImporter::getPrimitiveType() {
		if ((mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) != 0) {
			return "triangles";
		}
		//TODO: Support more primitive types. Right now all types except triangles are omitted during import
		//if ((mesh->mPrimitiveTypes & aiPrimitiveType_LINE) != 0) {
		//	return "lines";
		//}
		//if ((mesh->mPrimitiveTypes & aiPrimitiveType_POINT) != 0) {
		//	return "points";
		//}
		
		log.error("Mesh with name " + std::string(mesh->mName.C_Str()) + " has an unsupported primitive type. The mesh will be dropped.");
		return "Unsupported mesh type";
	}

} // End namespace AssimpWorker
