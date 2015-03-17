/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once
#include <iostream>
#include <ctime>

namespace AssimpWorker {
	class WorkUnit {
	public:
		WorkUnit(const std::string& workType, const std::string& importOperationId, const std::string& assetName);
		WorkUnit(const WorkUnit& other);
		~WorkUnit();

		const std::string workType;
		const std::string importOperationId;
		const std::string assetName;
		std::string sourcePath;
		std::string fileType;
		std::string resultPath;
		std::string detail;
		time_t startTime;
		time_t endTime;
	};
}
