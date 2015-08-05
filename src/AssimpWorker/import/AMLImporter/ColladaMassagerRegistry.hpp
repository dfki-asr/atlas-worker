/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <map>
#include "ColladaMassager.hpp"


namespace AssimpWorker {

	class ColladaMassagerRegistry
	{
	public:
		ColladaMassagerRegistry();

		~ColladaMassagerRegistry();

		ColladaMassager*  getMassager(const Poco::URI& colladaFileURI);

	private:
		std::map<std::string, ColladaMassager*> massagers;
	};

} // End namespace AssimpWorker


