/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <atlas/model/Asset.hpp>

namespace ATLAS {

namespace Model {

	Asset::Asset():
		Folder(*this, "node")
	{
		return;
	}

	Asset::~Asset(){
		for (auto& entry : blobMap) {
			entry.second->cleanup();
			delete entry.second;
		}
	}

	bool Asset::hasBlob(std::string hash){
		return blobMap.end() != blobMap.find(hash);
	}

	std::string Asset::addBlob(Blob& blob){
		std::string hash = blob.getHash();
		std::pair<std::string, Blob*> toInsert = std::make_pair(hash, blob.clone());
		blobMap.insert(toInsert);
		return hash;
	}

	Blob& Asset::getBlob(std::string hash) {
		return *((*(blobMap.find(hash))).second);
	}

	std::map<std::string, Blob*>& Asset::getBlobMap() {
		return blobMap;
	}

}

}
