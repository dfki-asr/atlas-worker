/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <string>
#include <exception>

namespace AssimpWorker {

	class Exception : public virtual std::exception{
	public:
		Exception(const std::string& msg);
		Exception(const Exception& other);
		virtual ~Exception() throw();

		void setMessage(const std::string& newMessage) throw();
		const std::string& getMessage() const throw();
	private:
		std::string message;

		//forbidden assignment operator
		Exception& operator=(const Exception& rhs);
	};

	std::ostream& operator<<(std::ostream& os, const Exception& e);

	class NotYetSupportedException : public Exception{
	public:
		NotYetSupportedException(const std::string& msg);
	};

	class InvalidJCRPathException : public Exception{
	public:
		InvalidJCRPathException(const std::string& msg);
	};

	class AMLException : public Exception{
	public:
		AMLException(const std::string& msg);
	};

} // End namespace AssimpWorker

