/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "../storage/JCRStorageService.hpp"
#include "WorkUnit.hpp"
#include "../worker/zipFileDecompressor.hpp"
#include "../import/Log.hpp"
#include "../import/BoostLog.hpp"

namespace AssimpWorker {
	class Worker {
	public:
		Worker();
		Worker(const Worker& other) = delete;
		~Worker();


		void import(WorkUnit& workUnit);

	private:
		JCRStorageService storageService;
		Log log;
		BoostLog boostLog;
		WorkUnit* currentWorkUnit;

		void importZipFile();
		void importSingleColladaFile();
		void decompressZipFile(const std::string& path, AssimpWorker::ZipFileExtractor& decompressor);
		std::string importAssetFromDecompressedZip(const std::string& path);
		std::string importColladaAndStore(const std::string& filesystemPathToColladaFile);
		std::string importAutomationMLAndStore(const std::string& filesystemPathToAMLFile);
		void ensureAssetFoldersExist(const std::string& assetPath);
		void sendFeedback(const std::string& message);
		std::string storeAsset(ATLAS::Model::Asset& asset);
	};
}
