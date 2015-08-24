/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/

#include <iostream>
#include "ColladaRecursiveImporter.hpp"
#include "../../internal/Exception.hpp"
#include <boost/format.hpp>

namespace AssimpWorker {

	using ATLAS::Model::DataDeletingBlob;
	using ATLAS::Model::Folder;

	ColladaRecursiveImporter::ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, const std::string& pathToWorkingDirectory, ColladaMassagerRegistry& registry, float scale) :
		Importer(colladaFileURI.toString(), log),
		colladaFileURI(colladaFileURI),
		childImporter(),
		importer(nullptr),
		massagerRegistry(registry),
		massager(nullptr),
		parentScale(scale)
	{
		return;
	}

	ColladaRecursiveImporter::ColladaRecursiveImporter(const Poco::URI& colladaFileURI, Log& log, const ColladaRecursiveImporter& parent) :
		Importer(colladaFileURI.toString(), log),
		colladaFileURI(colladaFileURI),
		childImporter(),
		importer(nullptr),
		massagerRegistry(parent.massagerRegistry),
		massager(nullptr),
		parentScale(parent.getLocalScale())
	{
		return;
	}

	ColladaRecursiveImporter::~ColladaRecursiveImporter(){
		delete importer;
		for (auto ci : childImporter){
			delete ci;
		}
	}

	void ColladaRecursiveImporter::addElementsTo(Folder& root){
		preprocessCollada();
		const aiScene* scene = runAssimpImport();
		convertToFolderStructure(scene, root);
		importChildColladas(root);
	}

	void ColladaRecursiveImporter::preprocessCollada(){
		massager = massagerRegistry.getMassager(colladaFileURI);
		massager->massage();
		if (parentScale != -1 && massager->getCurrentUnit() != parentScale) {
			throw Exception("Inconsistent scales used in input files.");
		}
	}

	const aiScene* ColladaRecursiveImporter::runAssimpImport(){
		importer = new AssimpWorker::AssimpImporter();
		const aiScene* scene = importer->importSceneFromFile(colladaFileURI.getPath(), log);
		if (!scene) {
			throw Exception("Error while running Assimp.");
		}
		return scene;
	}

	void ColladaRecursiveImporter::convertToFolderStructure(const aiScene* scene, Folder& root){
		const std::string fragment = colladaFileURI.getFragment();
		std::string colladaFileDirectory = extractDirectory(colladaFileURI);
		if (fragment != "") {
			aiNode* startingPointToImportFrom = scene->mRootNode->FindNode(fragment.c_str());
			if (startingPointToImportFrom == nullptr){
				throw Exception(boost::str(boost::format("Imported COLLADA is missing ID '%1%'") % fragment));
			}
			AiSceneImporter sceneImporter(scene, colladaFileDirectory, log);
			sceneImporter.importSubtreeOfScene(root, startingPointToImportFrom);
			Folder* startingPointToRestoreNames = findFolderWithName(root, fragment);
			if (startingPointToRestoreNames == nullptr){
				throw Exception(boost::str(boost::format("No Folder to restore Names for COLLADA ID '%1%'") % fragment));
			}
			massager->restoreOriginalNames(*startingPointToRestoreNames);
		}
		else {
			AiSceneImporter sceneImporter(scene, colladaFileDirectory, log);
			sceneImporter.addElementsTo(root);
			massager->restoreOriginalNames(root);
		}
	}

	void ColladaRecursiveImporter::importChildColladas(Folder& root){
		auto externalRefMap = massager->getExternalReferences();
		for (auto exRef : externalRefMap){
			Poco::URI uri(fixRelativeReference(exRef.second));
			ColladaRecursiveImporter* ci = new ColladaRecursiveImporter(uri, log, *this);
			childImporter.push_back(ci);
			Folder* entryPoint = findFolderWithColladaID(root, exRef.first);
			if (entryPoint == nullptr){
				throw Exception(
				            boost::str(
				                boost::format("No Folder '%1%' to import referenced document '%2%' into.")
				                % exRef.first
				                % exRef.second
				            )
				       );
			}
			ci->addElementsTo(*entryPoint);
		}
	}

	std::string ColladaRecursiveImporter::extractDirectory(const Poco::URI& fileURI)
	{
		const std::string& path = fileURI.getPath();
		return path.substr(0, path.find_last_of('/') + 1);
	}

	const std::string ColladaRecursiveImporter::getColladaUpAxis(){
		return massager->getUpAxis();
	}

	const float ColladaRecursiveImporter::getLocalScale() const {
		return massager->getCurrentUnit();
	}

	std::string ColladaRecursiveImporter::fixRelativeReference(const std::string relativeURIasString){
		std::string pathOfParentfile = colladaFileURI.getPath();
		auto lastSlashIndex = pathOfParentfile.find_last_of('/');
		pathOfParentfile.erase(lastSlashIndex+1);
		pathOfParentfile.append(relativeURIasString);
		return pathOfParentfile;
	}

	Folder* ColladaRecursiveImporter::findFolderWithName(Folder& folder, const std::string& name){
		if (name == folder.getName()){
			return &folder;
		}
		std::vector<Folder>& children = folder.getChildren();
		Folder* found = nullptr;
		for (Folder& child : children){
			found = findFolderWithName(child, name);
			if (found != nullptr){
				break;
			}
		}
		return found;
	}

	Folder* ColladaRecursiveImporter::findFolderWithColladaID(Folder& folder, const std::string& id){
		if (id == (folder).getAttribute("colladaID")){
			return &folder;
		}
		std::vector<Folder>& children = folder.getChildren();
		Folder* found = nullptr;
		for (Folder& child : children){
			found = findFolderWithColladaID(child, id);
			if (found != nullptr){
				break;
			}
		}
		return found;
	}
}
