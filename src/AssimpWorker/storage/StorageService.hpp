/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <atlas/model/Asset.hpp>

namespace AssimpWorker {
	class StorageService
	{
	public:
		StorageService();
		virtual ~StorageService();

		virtual std::istream& retrieveFile(const std::string& id) = 0;
		virtual std::string storeAssetRevision(const std::string& name, ATLAS::Model::Asset& asset) = 0;
		virtual std::string storeBlob(const std::string& name, ATLAS::Model::Blob& blob) = 0;

	private:
	};
}
