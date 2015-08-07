/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "ColladaImporter.hpp"

namespace AssimpWorker {

	using ATLAS::Model::Folder;

	ColladaImporter::ColladaImporter(const Poco::URI& url, Log& log, const Poco::URI& pathToWorkingDirectory) :
		Importer(url.toString(), log),
		colladaFileURI(url),
		pathToWorkingDirectory(pathToWorkingDirectory),
		importers(),
		massagerRegistry()
	{
		return;
	}

	ColladaImporter::~ColladaImporter(){
		return;
	}

	void ColladaImporter::addElementsTo(Folder& root){
		ColladaRecursiveImporter* colladaImporter = new ColladaRecursiveImporter(colladaFileURI, log, pathToWorkingDirectory, massagerRegistry, -1.0);
		importers.push_back(colladaImporter);
		colladaImporter->addElementsTo(root);
		fixScales(root, colladaImporter);
	}

	void ColladaImporter::fixScales(ATLAS::Model::Folder& root, ColladaRecursiveImporter* importer) {
		float localScale = importer->getLocalScale();
		aiMatrix4x4 transform = getTransformFor(root);
		aiMatrix4x4 scaling;
		aiMatrix4x4::Scaling(aiVector3t<float>(localScale), scaling);
		transform *= scaling;
		setTransformFor(root, transform);
	}

	aiMatrix4x4 ColladaImporter::getTransformFor(Folder& folder) {
		ATLAS::Model::Blob* currentFolderTransform = folder.getBlobByType("transform");
		if (currentFolderTransform) {
			return *(aiMatrix4x4*)currentFolderTransform->getData();
		}
		else {
			aiMatrix4x4 idendity;
			return idendity;
		}
	}

	void ColladaImporter::setTransformFor(Folder& folder, const aiMatrix4x4& newTransfrom) {
		ATLAS::Model::DataDeletingBlob<aiMatrix4x4> blob(new aiMatrix4x4(newTransfrom));
		folder.addBlob("transform", blob);
	}

} // End namespace AssimpWorker
