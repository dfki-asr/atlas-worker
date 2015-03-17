/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "zipFileDecompressor.hpp"

#include <iostream>
#include <Poco/Delegate.h>

namespace AssimpWorker {

	ZipFileExtractor::ZipFileExtractor(std::string decompressionPath):
		decompressionLocation(decompressionPath)
	{
		return;
	}

	void ZipFileExtractor::decompressArchive(std::istream& zipFile){
		Poco::Zip::Decompress decompressor(zipFile, decompressionLocation);
		auto errorHandler = Poco::Delegate<
				ZipFileExtractor,
				std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string>
			>(this, &ZipFileExtractor::onDecompressError);
		auto doneHandler = Poco::Delegate<
				ZipFileExtractor,
				std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path>
			>(this, &ZipFileExtractor::onDecompressDone);
		decompressor.EError += errorHandler;
		decompressor.EOk += doneHandler;
		decompressor.decompressAllFiles();
		decompressor.EError -= errorHandler;
		decompressor.EOk -= doneHandler;
		std::cout << "Decompression finished." << std::endl;
	}

#pragma warning(disable: 4100) //warning C4100 : 'pSender' : unreferenced formal parameter
	void ZipFileExtractor::onDecompressError(const void* pSender, std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string>& info) {
		std::cout << "Zip Decompress error: " << info.second << std::endl;
	}

#pragma warning(disable: 4100) //warning C4100 : 'pSender' : unreferenced formal parameter
	void ZipFileExtractor::onDecompressDone(const void* pSender, std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path>& info) {
		std::cout << "Decompressed: " << info.second.getFileName() << std::endl;
	}
}
