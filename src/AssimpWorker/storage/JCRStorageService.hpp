/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once
#pragma warning(push, 3)
#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/HTTPResponse.h>
#pragma warning(pop)
#include <jansson.h>
#include "JsonSerializer.hpp"
#include "StorageService.hpp"

namespace AssimpWorker {
	class JCRStorageService : public StorageService
	{
	public:
		JCRStorageService();
		virtual ~JCRStorageService();

		virtual std::istream& retrieveFile(const std::string& id);
		virtual std::string storeAssetRevision(const std::string& assetName, ATLAS::Model::Asset& asset);
		virtual std::string storeBlob(const std::string& assetName, ATLAS::Model::Blob& blob);

	private:
		std::string assetName;
		Poco::Net::HTTPClientSession session;
		Poco::Net::HTTPBasicCredentials credentials;
		std::string basePath;

		json_t* createEmptyJsonJCRForBlob();

		void openConnection();
		void ensureAssetFoldersExist();
		bool itemAlreadyExists(Poco::URI& uri);

		void postBlobItem(ATLAS::Model::Blob& blob);
		void postBlobBinary(ATLAS::Model::Blob& blob);
		void postAssetJSON(const char* jsonData, Poco::URI& uri);
		void postEmptyFolderUnderAsset(const std::string& folderName);

	protected:
		JsonSerializer jsonSerializer;
		std::string calculateHashForAsset(char* jsonData);
		void postBlobsForAsset(ATLAS::Model::Asset& asset);

	};
}
