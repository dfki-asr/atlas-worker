/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <istream>
#pragma warning (push,3)
#include <Poco/URI.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPResponse.h>
#pragma warning (pop)
#include <jansson.h>

namespace AssimpWorker {

	class EasyRequest {
		public:
			EasyRequest(Poco::Net::HTTPClientSession& session);
			EasyRequest(const EasyRequest& other) = delete;
			virtual ~EasyRequest();
			EasyRequest& get();
			EasyRequest& put();
			EasyRequest& post();
			EasyRequest& head();
			EasyRequest& del();
			EasyRequest& path(const Poco::URI& uri);
			EasyRequest& auth(const Poco::Net::HTTPBasicCredentials& cred);
			EasyRequest& type(const std::string& contentType);
			EasyRequest& sendContent(json_t* json);
			EasyRequest& sendContent(void* data, size_t length);
			std::istream& awaitResponse();
			Poco::Net::HTTPRequest& getRequest();
			Poco::Net::HTTPResponse& getResponse();
			bool isSuccess();
		private:
			Poco::Net::HTTPClientSession& session;
			Poco::Net::HTTPRequest* request;
			Poco::Net::HTTPResponse* response;
			void abortIfAlreadySent();
			void ensureRequestSent();
	};

}
