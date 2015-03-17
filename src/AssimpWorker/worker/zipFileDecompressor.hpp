/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <string>
#pragma warning ( push, 3 )
#pragma warning (disable : 4251)
	#include <Poco/Path.h>
	#include <Poco/Zip/Decompress.h>
#pragma warning ( pop )

namespace AssimpWorker {

	class ZipFileExtractor
	{
	public:
		ZipFileExtractor(std::string decompressionPath);

		void decompressArchive(std::istream& zipFile);

	private:
		Poco::Path decompressionLocation;
		void onDecompressError(const void* pSender, std::pair<const Poco::Zip::ZipLocalFileHeader, const std::string>& info);
		void onDecompressDone(const void* pSender, std::pair<const Poco::Zip::ZipLocalFileHeader, const Poco::Path>& info);
	};

} // End namespace AssimpWorker


