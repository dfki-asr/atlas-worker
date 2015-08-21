/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "../Importer.hpp"
#include "ColladaRecursiveImporter.hpp"
#include "ColladaMassager.hpp"
#include "ColladaMassagerRegistry.hpp"
#include <Poco/URI.h>

namespace AssimpWorker {

	class ColladaImporter : public Importer {
	public:
		ColladaImporter(const Poco::URI& url, Log& log, const std::string& pathToWorkingDirectory, ColladaMassagerRegistry& massagerRegistry);
		virtual ~ColladaImporter();
		virtual void addElementsTo(ATLAS::Model::Folder& root);

	private:
		const std::string& pathToWorkingDirectory;
		const Poco::URI& colladaFileURI;
		std::vector<ColladaRecursiveImporter*> importers;
		ColladaMassagerRegistry& massagerRegistry;
	};

} // End namespace AssimpWorker

