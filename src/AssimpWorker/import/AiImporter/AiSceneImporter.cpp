/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include "AiSceneImporter.hpp"
#include "AiNodeImporter.hpp"
#include "../../internal/Exception.hpp"
#include <atlas/model/Blob.hpp>

namespace AssimpWorker {

	using ATLAS::Model::Folder;
	using ATLAS::Model::Blob;
	using ATLAS::Model::DataUntouchingBlob;
	using ATLAS::Model::DataDeletingBlob;

	AiSceneImporter::AiSceneImporter(const aiScene* scene, const std::string& pathToFolder, Log& log) :
		Importer(pathToFolder, log),
		scene(scene),
		pathToFolder(pathToFolder)
	{
		return;
	}

	AiSceneImporter::~AiSceneImporter(){
		return;
	}

	void AiSceneImporter::addElementsTo(Folder& root){
		AiNodeImporter nodeImporter(scene, pathToFolder, scene->mRootNode, log);
		nodeImporter.addElementsTo(root);
		addLightsTo(root);
		addCamerasTo(root);
	}

	void AiSceneImporter::importSubtreeOfScene(Folder& root, aiNode* startingPoint){
		AiNodeImporter nodeImporter(scene, pathToFolder, startingPoint, log);
		nodeImporter.addElementsTo(root);
		addLightsTo(root);
		addCamerasTo(root);
	}

	void AiSceneImporter::addLightsTo(Folder& folder) {
		for (unsigned int i = 0; i < scene->mNumLights; ++i) {
			DataUntouchingBlob lightBlob(&scene->mLights[i], sizeof(aiLight));
			folder.addBlob("light", lightBlob);
		}
	}

	void AiSceneImporter::addCamerasTo(Folder& folder) {
		for (unsigned int i = 0; i < scene->mNumCameras; ++i) {
			DataUntouchingBlob cameraBlob(&scene->mCameras[i], sizeof(aiCamera));
			folder.addBlob("camera", cameraBlob);
		}
	}

} // End namespace AssimpWorker
