/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include "Poco/URI.h"
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Document.h>
#include <map>
#include <assimp/scene.h>
#include <atlas/model/Folder.hpp>

namespace AssimpWorker {

	class ColladaMassager
	{
	public:
		ColladaMassager(const Poco::URI& colladaPath);

		~ColladaMassager();

		void readXML();
		void massage();
		void restoreOriginalNames(aiNode* node);
		void restoreOriginalNames(ATLAS::Model::Folder& folder);

		std::vector<std::pair<std::string, std::string>>& getExternalReferences();
		const float getCurrentUnit() const;
		const std::string& getUpAxis() const;

	private:
		std::map<std::string, std::string> idToNameMap;
		Poco::URI uri;
		Poco::XML::AutoPtr<Poco::XML::Document> xmlDocument;
		bool needToPurge;
		std::vector<std::pair<std::string, std::string>> parentIDToExternalURL;
		int idCounter;
		bool alreadyMassagedMyFile;	
		float localScaleBeforeMassage;
		std::string upAxis;

		void readUpAxis();
		void extractExternalReferences();
		bool isInternalReference(const std::string& url);
		const std::string& getURLOfReference(Poco::XML::Node* node);
		const std::string getIDOfParentNode(Poco::XML::Node* node);
		void purgeAllNodes();
		void purgeNode(Poco::XML::Node* node);
		void writePurgedXML();
		std::string getNameForId(const std::string& id);
		void readUnit();
	};

} // End namespace AssimpWorker


