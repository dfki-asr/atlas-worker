#include "DiskStorageService.hpp"
#include "JCRStorageService.hpp"
#include "../internal/configuration.hpp"
#include <Poco/File.h>
#include <Poco/FileStream.h>
#include <iostream>

namespace AssimpWorker {
	
DiskStorageService::DiskStorageService()
{
	diskBasePath = Configuration::getInstance().get("disk-storage").as<std::string>();
}

std::string DiskStorageService::storeAssetRevision(const std::string& name, ATLAS::Model::Asset& asset)
{
	std::cout << "Storing asset " << name << " to Store " << diskBasePath << std::endl;
	ensureFoldersExist(name);
	json_t* assetJson = jsonSerializer.assetToJson(asset);
	char* jsonData = json_dumps(assetJson, JSON_COMPACT | JSON_PRESERVE_ORDER);
	std::string revisionHash = calculateHashForAsset(jsonData);

	std::map<std::string, ATLAS::Model::Blob*>& blobMap = asset.getBlobMap();
	for (auto entry : blobMap) {
		storeBlob(name, *(entry.second));
	}
	
	std::string path = diskBasePath + "/" + name + "/scene/" + revisionHash;
	Poco::File assetFile(path);	
	if (!assetFile.exists()) {
		Poco::FileOutputStream assetOutput(path);
		assetOutput << jsonData;
		assetOutput.close();
	}
	json_decref(assetJson);
	free(jsonData);
	return revisionHash;
}

std::string DiskStorageService::storeBlob(const std::string& name, ATLAS::Model::Blob& blob)
{
	std::string& hash = blob.getHash();
	std::string prefix = hash.substr(0,2);
	std::string suffix = hash.substr(2);
	std::string dir = diskBasePath + "/" + name + "/blobs/" + prefix + "/";
	std::string path = dir + suffix;

	std::cout << "Storing Blob " << path << " ...";

	Poco::File blobDir(dir);
	blobDir.createDirectories();
	
	Poco::File blobFile(path);
	if (!blobFile.exists()) {
		Poco::FileOutputStream blobOutput(path);
		blobOutput.write(static_cast<char*>(blob.getData()), blob.getDataSize());
		blobOutput.close();
		std::cout << " stored" << std::endl;
	} else {
		std::cout << " skipped (exists)" << std::endl;
	}
	return path;
}

void DiskStorageService::ensureFoldersExist(const std::string& assetName)
{
	Poco::File baseFolder(diskBasePath);
	if (!baseFolder.exists()) baseFolder.createDirectory();
	std::string assetPath = diskBasePath + "/" + assetName;
	Poco::File blobDirectory(assetPath + "/blobs/");
	if (!blobDirectory.exists()) blobDirectory.createDirectories();
	Poco::File sceneDirectory(assetPath + "/scene/");
	if (!sceneDirectory.exists()) sceneDirectory.createDirectory();
}

}
