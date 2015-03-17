/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "Exception.hpp"

namespace AssimpWorker {

	Exception::Exception(const std::string& msg):
		std::exception(),
		message(msg)
	{
		return;
	}

	Exception::Exception(const Exception& other):
		std::exception(),
		message(other.message)
	{
		return;
	}

	Exception::~Exception() throw(){
		return;
	}

	void Exception::setMessage(const std::string& newMessage) throw(){
		message = newMessage;
	}

	const std::string& Exception::getMessage() const throw(){
		return message;
	}

	std::ostream& operator<<(std::ostream& os, const Exception& e)
	{
		return os << e.getMessage();
	}



	NotYetSupportedException::NotYetSupportedException(const std::string& msg):
		Exception(msg)
	{
		return;
	}

	InvalidJCRPathException::InvalidJCRPathException(const std::string& msg) :
		Exception(msg)
	{
		return;
	}

	AMLException::AMLException(const std::string& msg) :
		Exception(msg)
	{
		return;
	}

} // End namespace AssimpWorker

