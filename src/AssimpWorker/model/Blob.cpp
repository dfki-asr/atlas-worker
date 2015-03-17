/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <stdlib.h>
#include <Poco/SHA1Engine.h>
#include <assimp/scene.h>
#include <atlas/model/AtlasMaterial.hpp>
#include <atlas/model/Blob.hpp>

namespace ATLAS {

namespace Model {

	Blob::Blob(void* data, uint32_t size) :
		data(data),
		dataSize(size)
	{
		invalidateHash();
	}

	Blob::~Blob(){
	}

	void* Blob::getData(){
		return this->data;
	}

	void Blob::setData(void* data, uint32_t size){
		this->data = data;
		this->dataSize = size;
		invalidateHash();
	}

	int Blob::getDataSize(){
		return this->dataSize;
	}

	std::string& Blob::getHash(){
		ensureHashComputed();
		return this->hash;
	}

	void Blob::invalidateHash() {
		this->hash = "";
	}

	void Blob::ensureHashComputed() {
		if (this->hash == "") {
			computeHash();
		}
	}

	void Blob::computeHash() {
		Poco::SHA1Engine engine;
		engine.update(this->data, this->dataSize);
		this->hash = Poco::DigestEngine::digestToHex(engine.digest());
	}

	DataFreeingBlob::DataFreeingBlob(void* data, uint32_t dataSize) :
		Blob(data, dataSize)
	{
		return;
	}

	void DataFreeingBlob::cleanup(){
		free(data);
	}

	DataFreeingBlob* DataFreeingBlob::clone() const{
		return new DataFreeingBlob(*this);
	}

	DataUntouchingBlob::DataUntouchingBlob(void* data, uint32_t dataSize) :
		Blob(data, dataSize)
	{
		return;
	}

	void DataUntouchingBlob::cleanup(){
		return;
	}

	DataUntouchingBlob* DataUntouchingBlob::clone() const{
		return new DataUntouchingBlob(*this);
	}

}

}
