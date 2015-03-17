/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <Poco/SHA1Engine.h>
#include "JCRStorageService.hpp"
#include "../internal/configuration.hpp"
#include "../internal/Exception.hpp"
#include "../connection/EasyRequest.hpp"


namespace AssimpWorker {

	using ATLAS::Model::Asset;
	using ATLAS::Model::Blob;

	JCRStorageService::JCRStorageService() :
		assetName(),
		session(),
		credentials()
	{
		openConnection();
	}

	JCRStorageService::~JCRStorageService() {
	}

//////////////////////////// Public API

	std::istream& JCRStorageService::retrieveFile(const std::string& id) {
		Poco::URI uri(basePath + "binary/scratch/" + id + "/jcr:content/jcr:data");
		std::cout << "Retrieving: " << uri.toString() << std::endl;
		EasyRequest request(session);
		std::istream& fileStream = request
			.get()
			.auth(credentials)
			.path(uri)
			.awaitResponse();
		if (!request.isSuccess()) {
			std::cerr << "Failed to retrieve: " << request.getResponse().getStatus()
			          << " " << request.getResponse().getReason() << std::endl;
			throw Exception("Error retrievening File from JCR.");
		}
		return fileStream;
	}

	std::string JCRStorageService::storeAssetRevision(const std::string& name, Asset& asset) {
		assetName = name;
		ensureAssetFoldersExist();
		json_t* assetJson = jsonSerializer.assetToJson(asset);
		char* jsonData = json_dumps(assetJson, JSON_COMPACT | JSON_PRESERVE_ORDER);
		std::string revisionHash = calculateHashForAsset(jsonData);
		std::string path = basePath + "items/assets/" + assetName + "/scene/" + revisionHash;
		Poco::URI assetURI(path);
		postBlobsForAsset(asset);
		if (!itemAlreadyExists(assetURI)) {
			postAssetJSON(jsonData, assetURI);
		}
		json_decref(assetJson);
		free(jsonData);
		return revisionHash;
	}

	std::string JCRStorageService::storeBlob(const std::string& name, Blob& blob) {
		assetName = name;
		std::string path = basePath + "items/assets/" + assetName + "/blobs/" + blob.getHash();
		std::cout << "Storing Blob " << blob.getHash() << " ...";
		Poco::URI itemURI(path);
		if (!itemAlreadyExists(itemURI)) {
			postBlobItem(blob);
			postBlobBinary(blob);
			std::cout << " stored" << std::endl;
		} else {
			std::cout << " skipped (exists)" << std::endl;
		}
		return path.append("/jcr:content/jcr:data");
	}


////////////////////////////// Private functions

	void JCRStorageService::openConnection() {
		Configuration& config = Configuration::getInstance();
		basePath = config.get("jcr-url").as<std::string>();
		Poco::URI uri(basePath);
		session.setHost(uri.getHost());
		session.setPort(uri.getPort());
		credentials.setUsername(config.get("jcr-user").as<std::string>());
		credentials.setPassword(config.get("jcr-pass").as<std::string>());
	}

	void JCRStorageService::postBlobItem(Blob& blob) {
		Poco::URI uri(basePath + "items/assets/" + assetName + "/blobs/" + blob.getHash());
		json_t* folder = createEmptyJsonJCRForBlob();
		EasyRequest request(session);
		request.post()
			   .auth(credentials)
			   .path(uri)
			   .type("application/json")
			   .sendContent(folder);
		request.awaitResponse();
		json_decref(folder);
	}

	void JCRStorageService::postBlobBinary(Blob& blob) {
		Poco::URI uri(basePath + "binary/assets/" + assetName + "/blobs/" + blob.getHash() + "/jcr:content/jcr:data");
		EasyRequest request(session);
		request.post()
			   .auth(credentials)
			   .path(uri)
			   .type("application/octet-stream")
			   .sendContent(blob.getData(), blob.getDataSize());
		request.awaitResponse();
	}

	void JCRStorageService::postBlobsForAsset(Asset& asset) {
		std::map<std::string, Blob*>& blobMap = asset.getBlobMap();
		for (auto entry : blobMap) {
			storeBlob(assetName, *(entry.second));
		}
	}

	void JCRStorageService::postAssetJSON(const char* jsonData, Poco::URI& uri) {
		json_t* folder = json_object();
		json_object_set_new(folder, "jcr:primaryType", json_string("nt:unstructured"));
		json_object_set_new(folder, "scene", json_string(jsonData));
		EasyRequest request(session);
		request.post()
			.auth(credentials)
			.path(uri)
			.type("application/json")
			.sendContent(folder);
		request.awaitResponse();
		json_decref(folder);
	}

	void JCRStorageService::postEmptyFolderUnderAsset(const std::string& folder) {
		json_t* folderJSON = json_object();
		json_object_set_new(folderJSON, "jcr:primaryType", json_string("nt:unstructured"));
		Poco::URI uri(basePath + "items/assets/" + assetName + "/" + folder);
		EasyRequest request(session);
		request.post()
			.auth(credentials)
			.path(uri)
			.type("application/json")
			.sendContent(folderJSON);
		request.awaitResponse();
		json_decref(folderJSON);
	}

	bool JCRStorageService::itemAlreadyExists(Poco::URI& uri) {
		EasyRequest request(session);
		request
			.head()
			.auth(credentials)
			.path(uri)
			.awaitResponse();
		return request.isSuccess();
	}

	std::string JCRStorageService::calculateHashForAsset(char* jsonData) {
		Poco::SHA1Engine engine;
		engine.update(jsonData, (unsigned int) strlen(jsonData));
		return Poco::DigestEngine::digestToHex(engine.digest());
	}

	void JCRStorageService::ensureAssetFoldersExist() {
		Poco::URI uri(basePath + "items/assets/" + assetName);
		if (!itemAlreadyExists(uri)) {
			postEmptyFolderUnderAsset("");
			postEmptyFolderUnderAsset("blobs/");
			postEmptyFolderUnderAsset("scene/");
		}
	}

	json_t* JCRStorageService::createEmptyJsonJCRForBlob() {
		json_t* folder = json_object();
		json_object_set_new(folder, "jcr:primaryType", json_string("nt:unstructured"));
		json_t* content = json_object();
		json_object_set_new(content, "jcr:primaryType", json_string("nt:resource"));
		json_object_set_new(content, "jcr:data", json_string("no_data"));
		json_t* children = json_object();
		json_object_set_new(children, "jcr:content", content);
		json_object_set_new(folder, "children", children);
		return folder;
	}
}
