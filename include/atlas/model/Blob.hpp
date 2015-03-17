/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <inttypes.h>
#include <string>

namespace ATLAS {

namespace Model {

	class Blob {
	protected:
		Blob(void* data, uint32_t size);
	public:
		//* The destructor of Blob does not free the memory pointed to by getData().
		//* If you need to free data after we're done, implement cleanup().
		virtual ~Blob();

		void* getData();
		void setData(void* data, uint32_t size);
		int getDataSize();
		std::string& getHash();
		void invalidateHash();

		//* This is supposed to be called by the BlobHolder when said BlobHolder is destroyed.
		//* If you need to free data, here's the place to do that.
		virtual void cleanup() = 0;

		virtual Blob* clone() const = 0;

	protected:
		void* data;
		uint32_t dataSize;
		std::string hash;

		void ensureHashComputed();
		void computeHash();
	};

	class DataFreeingBlob : public Blob{
	public:
		DataFreeingBlob(void* data, uint32_t dataSize);
		virtual void cleanup();
		virtual DataFreeingBlob* clone() const;
	};

	template<class ObjectType>
	class DataDeletingBlob : public Blob {
	public:
		DataDeletingBlob(ObjectType* data) :
			Blob(data, sizeof(ObjectType))
		{
			return;
		}

		virtual void cleanup(){
			delete (ObjectType*)data;
		}
		virtual DataDeletingBlob<ObjectType>* clone() const{
			// use default copy constructor
			return new DataDeletingBlob<ObjectType>(*this);
		}
	};

	class DataUntouchingBlob : public Blob{
	public:
		DataUntouchingBlob(void* data, uint32_t dataSize);
		virtual void cleanup();
		virtual DataUntouchingBlob* clone() const;
	};

}

}
