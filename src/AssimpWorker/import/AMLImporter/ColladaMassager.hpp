/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "ColladaMassager.hpp"
#include "Poco/URI.h"
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Document.h>
#include <map>
#include "../Log.hpp"

namespace AssimpWorker {

	class ColladaMassager
	{
	public:
		ColladaMassager(const Poco::URI& colladaPath, Log& externalLog);

		~ColladaMassager();

		void purgeNames();
		std::string getNameForId(const std::string& id);

	private:
		std::map<std::string, std::string> idToNameMap;
		Poco::URI uri;
		Poco::XML::AutoPtr<Poco::XML::Document> xmlDocument;
		Log log;

		void readXML();
		void purgeAllNodes();
		void purgeNode(Poco::XML::Node* node);
		void writePurgedXML();
	};

} // End namespace AssimpWorker


