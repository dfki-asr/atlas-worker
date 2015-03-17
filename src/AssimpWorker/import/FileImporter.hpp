/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <atlas/model/Folder.hpp>
#include "Log.hpp"

namespace AssimpWorker {

	class FileImporter{
	public:
		FileImporter (const std::string& url, Log& log);
		virtual ~FileImporter();
		void setFileType(const std::string& type);
		virtual void addElementsTo(ATLAS::Model::Folder& root);
	protected:
		Log& log;
		const std::string& url;
		std::string fileType;

		void addFileBlob(ATLAS::Model::Folder& root);
		std::string getMimetypeFromFilePath();
		bool readFile(char** destination, uint32_t* size);
	};

} // End namespace AssimpWorker

