/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "Importer.hpp"

namespace AssimpWorker {

	/////////////////////////// Importer

	Importer::Importer(const std::string& url, Log& externalLog) :
		log(externalLog)
	{
		return;
	}

	Importer::~Importer(){
		return;
	}

} // End namespace AssimpWorker
