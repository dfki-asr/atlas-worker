/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "Folder.hpp"

namespace ATLAS {

namespace Model {

	class Asset : public BlobHolder, public Folder{
	public:
		Asset();
		virtual ~Asset();

		virtual bool hasBlob(std::string hash);
		virtual std::string addBlob(Blob& blob);
		virtual Blob& getBlob(std::string hash);
		virtual std::map<std::string, Blob*>& getBlobMap();
	private:
		std::map<std::string, Blob*> blobMap;
	};

}

}
