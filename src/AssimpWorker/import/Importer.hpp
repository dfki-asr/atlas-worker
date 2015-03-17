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

	class Importer{
	public:
		Importer(const std::string& url, Log& log);
		virtual ~Importer();
		virtual void addElementsTo(ATLAS::Model::Folder& root) = 0;
	protected:
		Log& log;
	};

} // End namespace AssimpWorker

