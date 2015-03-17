/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <Poco/URI.h>
#include <jansson.h>
#include <atlas/model/Asset.hpp>

namespace AssimpWorker {
	class JsonSerializer
	{
	public:
		JsonSerializer();
		virtual ~JsonSerializer();

		json_t* assetToJson(ATLAS::Model::Asset& asset);

	private:
		json_t* stringToJson(const std::string& jsonString);
		json_t* folderToJson(const ATLAS::Model::Folder& folder);
		json_t* blobToJson(ATLAS::Model::Blob& blob);
		json_t* createJsonFromFolder(const ATLAS::Model::Folder& folder);
		void addChildrenToJson(const ATLAS::Model::Folder& folder, json_t* parentJSON);
		void addFolderToParent(json_t* json_folder, json_t* parent);
		void addBlobsToJson(const ATLAS::Model::Folder& folder, json_t* json);
		void addAttributesToJson(const ATLAS::Model::Folder& folder, json_t* json);
	};
}
