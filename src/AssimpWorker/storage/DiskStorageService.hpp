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
#include "JCRStorageService.hpp"

namespace AssimpWorker {

class DiskStorageService : public JCRStorageService
{
	public:
		DiskStorageService();
		
		std::string storeAssetRevision(const std::string& name, ATLAS::Model::Asset& asset);
		std::string storeBlob(const std::string& name, ATLAS::Model::Blob& blob);

	private:
		std::string diskBasePath;
		void ensureFoldersExist(const std::string& assetName);
};

}
