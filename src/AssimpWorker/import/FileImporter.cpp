/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "FileImporter.hpp"
#include <iostream>
#include <fstream>
#include <atlas/model/Blob.hpp>
#include "../internal/Exception.hpp"

namespace AssimpWorker {

	/////////////////////////// Importer

	FileImporter::FileImporter(const std::string& url, Log& externalLog) :
		log(externalLog),
		url(url),
		fileType()
	{
		return;
	}

	FileImporter::~FileImporter(){
		return;
	}

	void FileImporter::setFileType(const std::string& type){
		this->fileType = type;
	}

	void FileImporter::addElementsTo(ATLAS::Model::Folder& root){
		if (this->fileType.empty()){
			throw Exception("Must set fileType befor adding File to Folder.");
		}
		char* data;
		uint32_t size;
		const std::string mimeType = getMimetypeFromFilePath();
		if (readFile(&data, &size)) {
			ATLAS::Model::DataFreeingBlob fileBlob(data, size);
			root.addBlob(this->fileType, fileBlob);
			root.addAttribute("mimetype", mimeType);
		}
	}

	std::string FileImporter::getMimetypeFromFilePath() {
		auto fileSuffixIndex = this->url.find_last_of(".");
		return std::string("image/" + this->url.substr(fileSuffixIndex + 1));
	}

	bool FileImporter::readFile(char** destination, uint32_t* size) {
		std::ifstream file;
		try {
			file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			file.open(this->url, std::ios::in | std::ios::binary | std::ios::ate);
#pragma warning (disable : 4244) //warning C4244: '=' : conversion from 'std::streamoff' to 'uint32_t', possible loss of data
			*size = file.tellg();
			*destination = (char*)malloc(*size * sizeof(uint8_t));
			file.seekg(0, std::ios::beg);
			file.read(*destination, *size);
			return true;
		}
		catch (const std::ifstream::failure& e) {
			log.error("Could not open texture file '" + this->url + "', texture will not be saved.");
			return false;
		}
	}

} // End namespace AssimpWorker
