/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "ColladaImporter.hpp"

namespace AssimpWorker {

	using ATLAS::Model::Folder;

	ColladaImporter::ColladaImporter(const Poco::URI& url, Log& log) :
		Importer(url.toString(), log),
		colladaFileURI(url),
		importers(),
		massagerRegistry()
	{
		return;
	}

	ColladaImporter::~ColladaImporter(){
		for (auto i : importers){
			delete i;
		}
	}

	void ColladaImporter::addElementsTo(Folder& root){
		ColladaRecursiveImporter* colladaImporter = new ColladaRecursiveImporter(colladaFileURI, log, massagerRegistry);
		importers.push_back(colladaImporter);
		colladaImporter->addElementsTo(root);
	}

} // End namespace AssimpWorker
