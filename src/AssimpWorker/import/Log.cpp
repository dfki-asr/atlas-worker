/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "Log.hpp"
#include <sstream>

namespace AssimpWorker {

Log::Log() {

}

void Log::error(const char* message) {
	errors.push_back(message);
}

void Log::error(const std::string& message) {
	errors.push_back(message);
}

void Log::info(const char* message) {
	infos.push_back(message);
}

void Log::info(const std::string& message) {
	infos.push_back(message);
}

std::string Log::getAllErrors() {
	std::stringstream out;
	for (auto errorMessage : errors) {
		out << errorMessage << std::endl;
	}
	return out.str();
}

std::string Log::getAllInfos() {
	std::stringstream out;
	for (auto infoMessage : infos) {
		out << infoMessage << std::endl;
	}
	return out.str();
}

}
