/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <atlas/model/Folder.hpp>
#include <boost/bind.hpp>
#include <boost/mem_fn.hpp>

namespace ATLAS {

namespace Model {

	Folder::Folder(BlobHolder& blobHolder, const std::string& type) :
		blobHolder(blobHolder),
		type(type)
	{
		return;
	}

	// If this were pure C++11, we'd use =default; in the header,
	// but silly MSVC wants the default move constructor spelled out.
	Folder::Folder(Folder&& folder) : 
		blobHolder(folder.blobHolder),
		name(std::move(folder.name)),
		type(std::move(folder.type)),
		blobs(std::move(folder.blobs)),
		attributes(std::move(folder.attributes)),
		childFolders(std::move(folder.childFolders))
	{

	}

	Folder& Folder::operator=(Folder&& other) {
		if (type != other.type) {
			// must not assign different types of folders.
			throw std::exception();
		}
		blobHolder = other.blobHolder;
		name = std::move(other.name);
		blobs = std::move(other.blobs);
		attributes = std::move(other.attributes);
		childFolders = std::move(other.childFolders);
		return *this;
	}

	Folder::~Folder(){
		return;
	}

	const std::string& Folder::getName() const{
		return name;
	}

	void Folder::setName(const std::string& newName){
		name = newName;
	}

	const std::string& Folder::getType() const{
		return type;
	}

	Folder& Folder::appendChild(const std::string& type){
		Folder newFolder(blobHolder, type);
		childFolders.push_back(std::move(newFolder));
		return childFolders.back();
	}

	const std::vector<Folder>& Folder::getChildren() const{
		return childFolders;
	}

	Blob* Folder::getBlobByType(const std::string& type){
		for (auto type_and_hash : blobs){
			if (type_and_hash.first == type) {
				return &(blobHolder.getBlob(type_and_hash.second));
			}
		}
		return NULL;
	}

	void Folder::addBlob(const std::string& type, Blob& blob){
		std::string hash = blob.getHash();
		if (!blobHolder.hasBlob(hash)){
			blobHolder.addBlob(blob);
		}
		blobs.erase(type); // Re-adding a blob should overwrite it
		blobs.insert(std::pair<std::string, std::string>(type,hash));
	}

	bool Folder::hasChildren() const{
		return !childFolders.empty();
	}

	void Folder::addAttribute(const std::string& key, const std::string& value) {
		attributes.insert(std::pair<std::string, std::string>(key, value));
	}

	std::string Folder::getAttribute(const std::string& key) {
		auto valueIterator = attributes.find(key);
		if (valueIterator != attributes.end()) {
			return valueIterator->second;
		}
		else {
			return "ATTRIBUTE_NOT_FOUND";
		}
	}

	std::map<std::string, std::string>& Folder::getAttributes(){
		return attributes;
	}

	const std::map<std::string, std::string>& Folder::getAttributes() const{
		return attributes;
	}

	Folder::iterator Folder::begin() {
		return blobs.begin();
	}

	Folder::iterator Folder::end() {
		return blobs.end();
	}

	Folder::const_iterator Folder::begin() const{
		return blobs.begin();
	}

	Folder::const_iterator Folder::end() const{
		return blobs.end();
	}

}

}
