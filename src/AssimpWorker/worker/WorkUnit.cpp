/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "WorkUnit.hpp"

namespace AssimpWorker {

	WorkUnit::WorkUnit(const std::string& workType, const std::string& importOperationId, const std::string& assetName) :
		workType(workType),
		importOperationId(importOperationId),
		assetName(assetName),
		sourcePath(""),
		fileType(""),
		resultPath(""),
		detail(""),
		startTime(0),
		endTime(0)
	{
		return;
	}

	WorkUnit::WorkUnit(const WorkUnit& other) :
		workType(other.workType),
		importOperationId(other.importOperationId),
		assetName(other.assetName),
		sourcePath(other.sourcePath),
		fileType(other.fileType),
		resultPath(other.resultPath),
		detail(other.detail),
		startTime(other.startTime),
		endTime(other.endTime)
	{

	}

	WorkUnit::~WorkUnit()
	{
		return;
	}

}
