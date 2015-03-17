/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <sstream>
#include <assimp/DefaultLogger.hpp>
#include <assimp/version.h>
#include "AssimpVersion.hpp"

namespace AssimpWorker {

std::string AssimpVersion::getAssimpVersion() {
	unsigned int major = aiGetVersionMajor();
	unsigned int minor = aiGetVersionMinor();
	unsigned int rev = aiGetVersionRevision();
	bool debug = (aiGetCompileFlags() & ASSIMP_CFLAGS_DEBUG) != 0;

	std::stringstream versionString;

	versionString << "AssImp v" << major << "." << minor << "." << rev;
	if (debug) {
		versionString << " (DEBUG)";
	}
	return versionString.str();
}

void AssimpVersion::printAssimpVersion() {
	std::cout << getAssimpVersion() << std::endl;
}

}
