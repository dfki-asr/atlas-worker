/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <string>
#include <vector>

namespace AssimpWorker {

	class Log {
		public:
			Log();
			void error(const char* message);
			void error(const std::string& message);
			void info(const char* message);
			void info(const std::string& message);
			std::string getAllErrors();
			std::string getAllInfos();
		private:
			std::vector<std::string> errors;
			std::vector<std::string> infos;
	};

}
