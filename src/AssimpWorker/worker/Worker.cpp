/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <fstream>
#include <Poco/Net/NetException.h>
#include <Poco/TemporaryFile.h>
#include <boost/filesystem.hpp>
#include "../import/AssimpImporter.hpp"
#include "../import/AiImporter/AiSceneImporter.hpp"
#include "../import/AMLImporter/AMLImporter.hpp"
#include <atlas/model/Asset.hpp>
#include "../internal/configuration.hpp" 
#include "../connection/FeedbackProducer.hpp"
#include "Worker.hpp"
#include "../internal/Exception.hpp"

namespace AssimpWorker {

	using ATLAS::Model::Asset;

	Worker::Worker() : 
		storageService(),
		currentWorkUnit(NULL)
	{
		return;
	}

	Worker::~Worker()
	{
		return;
	}

	void Worker::import(WorkUnit& workUnit) {
		try {
			currentWorkUnit = &workUnit;
			time(&currentWorkUnit->startTime);
			sendFeedback("processing");

			if (currentWorkUnit->fileType == "zip") {
				std::cout << "File is a zip, decompressing..." << std::endl;
				importZipFile();
			}
			else if (currentWorkUnit->fileType == "dae") {
				std::cout << "File is a Collada file, importing..." << std::endl;
				importSingleColladaFile();
			}

			currentWorkUnit->detail = log.getAllErrors();
			std::cout << currentWorkUnit->detail << std::endl;
			time(&currentWorkUnit->endTime);

			if (currentWorkUnit->resultPath.empty()) {
				sendFeedback("failed");
			} else {
				sendFeedback("complete");
			}
		}
		catch (const Poco::Net::ConnectionRefusedException& ex) {
			std::cerr << "Error connecting to the ATLAS server: " << ex.displayText() << std::endl << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "Error during import operation: " << e.what() << std::endl << std::endl;
		}
		catch (...) {
			std::cerr << "Unknown error during import operation." << std::endl << std::endl;
		}
	}

	void Worker::importSingleColladaFile() {
		std::istream& response = storageService.retrieveFile(currentWorkUnit->sourcePath);
		// Patches welcome...
		std::string decompressionPath = Configuration::getInstance().get("decompression-path").as<std::string>();
		Poco::TemporaryFile tmp(decompressionPath);
		std::ofstream ostr;
		ostr.open(tmp.path().c_str());
		std::streamsize n;
		char buffer[1024] = { 0x0 };
		response.read(buffer, 1024);
		while ((n = response.gcount()) != 0) {
			ostr.write(buffer, n);
			if (n) {
				response.read(buffer, 1024);
			}
		}
		ostr.close();
		currentWorkUnit->resultPath = importColladaAndStore(tmp.path());
	}

	void Worker::importZipFile() {
		Configuration& config = Configuration::getInstance();
		std::string decompressionPath = config.get("decompression-path").as<std::string>();
		AssimpWorker::ZipFileExtractor decompressor(decompressionPath);
		decompressZipFile(currentWorkUnit->sourcePath, decompressor);
		std::cout << "Decompression complete, beginning import..." << std::endl;
		currentWorkUnit->resultPath = importAssetFromDecompressedZip(decompressionPath);
		boost::filesystem::remove_all(decompressionPath);
	}

	void Worker::decompressZipFile(const std::string& zipId, AssimpWorker::ZipFileExtractor& decompressor) {
		std::istream& response = storageService.retrieveFile(zipId);
		decompressor.decompressArchive(response);
	}

	std::string Worker::importColladaAndStore(const std::string& filesystemPathToColladaFile) {
		std::cout << "importColladaAndStore, filesystemPathToColladaFile: " << filesystemPathToColladaFile << std::endl;
		std::string pathToFolder = filesystemPathToColladaFile.substr(0, filesystemPathToColladaFile.find_last_of('/') + 1);
		Poco::URI& uri = Poco::URI(filesystemPathToColladaFile);
		AssimpWorker::ColladaImporter importer = AssimpWorker::ColladaImporter(uri, log, pathToFolder);
		Asset asset;
		importer.addElementsTo(asset);
		return storeAsset(asset);
	}

	std::string Worker::storeAsset(Asset& asset) {
		asset.setName(currentWorkUnit->assetName);
		std::cout << "Conversion done, storing..." << std::endl;
		std::string revisionHash = storageService.storeAssetRevision(currentWorkUnit->assetName, asset);
		std::string relativePathToAsset = currentWorkUnit->assetName + "/" + revisionHash;
		std::cout << "Saved asset to path " << relativePathToAsset << std::endl;
		return relativePathToAsset;
	}

	std::string Worker::importAutomationMLAndStore(const std::string& filesystemPathToAMLFile) {
		try
		{
			AMLImporter amlImporter(filesystemPathToAMLFile, log);
			Asset asset;
			amlImporter.addElementsTo(asset);
			return storeAsset(asset);
		}
		catch (const Exception& e) {
			log.error("Error while processing the AML: " + e.getMessage());
			return "";
		}
	}

	std::string Worker::importAssetFromDecompressedZip(const std::string& path){
		boost::filesystem::directory_iterator end;
		boost::filesystem::directory_iterator it(path);
		std::string daePath = "";
		for (; it != end; ++it) {
			std::string extension = boost::filesystem::extension(*it);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			// Any AML file should take priority over Collada files, so we delay Collada importing until all extensions are checked
			if (extension == ".dae" && daePath == "") {
				daePath = it->path().generic_string();
			}
			if (extension == ".aml") {
				return importAutomationMLAndStore(it->path().generic_string());
			}
		}
		if (daePath != "") {
			return importColladaAndStore(daePath);
		}
		log.error("Could not find a supported file type to convert. Make sure the format you are trying to import is supported by ATLAS. Also, remember: Folders are not supported by ATLAS, make sure the file to be imported is within the toplevel of the zip-file.");
		return "";
	}

	void Worker::sendFeedback(const std::string& message) {
		//Producer connection is created on demand to avoid having to heartbeat it
		FeedbackProducer producer;
		producer.sendWorkFeedbackMessage(*currentWorkUnit, message);
	}

}
