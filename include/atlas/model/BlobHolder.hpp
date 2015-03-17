/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <string>
#include "Blob.hpp"

namespace ATLAS {

namespace Model {

	// "Interface" BlobHolder, a collection of blobs, accessible by hash.
	class BlobHolder{
	public:
		BlobHolder(){};
		virtual ~BlobHolder() {};

		virtual bool hasBlob(std::string hash) = 0;
		virtual std::string addBlob(Blob& blob) = 0;
		virtual Blob& getBlob(std::string hash) = 0;
		virtual std::map<std::string, Blob*>& getBlobMap() = 0;
	};

}

}
