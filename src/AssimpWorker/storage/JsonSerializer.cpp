/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include "JsonSerializer.hpp"

namespace AssimpWorker {

	using ATLAS::Model::Asset;
	using ATLAS::Model::Folder;

	JsonSerializer::JsonSerializer() {

	}

	JsonSerializer::~JsonSerializer() {

	}

	json_t* JsonSerializer::stringToJson(const std::string& jsonString) {
		json_error_t err;
		json_t* data = json_loads(jsonString.c_str(), 0, &err);
		if (!data) {
			std::cerr << "Could not convert string to json: " << err.text;
		}
		return data;
	}

	json_t* JsonSerializer::assetToJson(Asset& asset) {
		json_t* root = createJsonFromFolder(asset);
		addBlobsToJson(asset, root);
		addAttributesToJson(asset, root);
		addChildrenToJson(asset, root);
		return root;
	}

	json_t* JsonSerializer::folderToJson(const Folder& folder){
		json_t* json_folder = createJsonFromFolder(folder);
		addAttributesToJson(folder, json_folder);
		return json_folder;
	}

	json_t* JsonSerializer::createJsonFromFolder(const Folder& folder){
		return json_pack("{ s:s, s:s, s:[], s:{}, s:{} }",
				"name", folder.getName().c_str(),
				"type", folder.getType().c_str(),
				"children",
				"attributes",
				"blobs"
				);
	}

	void JsonSerializer::addChildrenToJson(const Folder& folder, json_t* parentJSON){
		if (folder.hasChildren()){
			const std::vector<Folder>& children = folder.getChildren();
			for (const Folder& child : children){
				json_t* json = folderToJson(child);
				addBlobsToJson(child, json);
				addChildrenToJson(child, json);
				addFolderToParent(json, parentJSON);
			}
		}
	}

	void JsonSerializer::addFolderToParent(json_t* json_folder, json_t* parent){
		json_t* children = json_object_get(parent, "children");
		json_array_append_new(children, json_folder);
	}

	void JsonSerializer::addBlobsToJson(const Folder& folder, json_t* json){
		json_t* jsonBlobs = json_object_get(json, "blobs");
		for (auto type_and_hash : folder){
			auto jsonKey = type_and_hash.first.c_str();
			json_t* jsonValue = json_string(type_and_hash.second.c_str());
			json_object_set_new(jsonBlobs, jsonKey, jsonValue);
		}
	}

	void JsonSerializer::addAttributesToJson(const Folder& folder, json_t* json){
		auto& attributes = folder.getAttributes();
		json_t* jsonAttributes = json_object_get(json, "attributes");
		for (auto& attribute : attributes){
			auto& jsonKey = attribute.first;
			json_t* jsonValue = json_string(attribute.second.c_str());
			json_object_set_new(jsonAttributes, jsonKey.c_str(), jsonValue);
		}
	}
}

