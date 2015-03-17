/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <map>
#include <string>
#include <vector>
#include "Blob.hpp"
#include "BlobHolder.hpp"

namespace ATLAS {

namespace Model {

	class Folder {
		typedef std::map<std::string, std::string> BlobHashList;
	public:
		Folder(BlobHolder& blobHolder, const std::string& type);
		// expressly forbid copy construction since we like to forget ampersands.
		Folder(const Folder&) = delete;
		// we use move construction internally, thus this needs to be re-enabled.
		// it's also not quite as bad in terms of accidental memory leaks as the copy-constructor.
		Folder(Folder&&);

		virtual ~Folder();

		const std::string& getType() const;
		const std::string& getName() const;
		void setName(const std::string& newName);
		bool hasChildren() const;
		Folder& appendChild(const std::string& type);
		const std::vector<Folder>& getChildren() const;
		Blob* getBlobByType(const std::string& type);
		void addBlob(const std::string& type, Blob& blob);
		void addAttribute(const std::string& key, const std::string& value);
		std::string getAttribute(const std::string& key);
		std::map<std::string, std::string>& getAttributes();
		const std::map<std::string, std::string>& getAttributes() const;

	protected:
		BlobHolder& blobHolder;
		std::string name;
		const std::string type;
		BlobHashList blobs;
		std::map<std::string, std::string> attributes;
		std::vector<Folder> childFolders;

	public:
		typedef BlobHashList::iterator iterator;
		typedef BlobHashList::const_iterator const_iterator;
		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;
	};

}

}
