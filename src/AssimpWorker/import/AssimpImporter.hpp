/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

//This needs to be the first assimp include. Without this include other assimp includes will just break apart. Afaik ...
#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include "Log.hpp"
#include "BoostLog.hpp"

namespace AssimpWorker {

	class AssimpImporter
	{
	public:
		AssimpImporter();

		~AssimpImporter();

		const aiScene* importSceneFromFile(std::string fileName, Log& log);

	private:
		Assimp::Importer importer;
	};

} // End namespace AssimpWorker


