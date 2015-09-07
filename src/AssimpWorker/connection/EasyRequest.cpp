/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "EasyRequest.hpp"
#include "../internal/Exception.hpp"

namespace AssimpWorker {

EasyRequest::EasyRequest(Poco::Net::HTTPClientSession& session) :
	session(session),
	request(new Poco::Net::HTTPRequest()),
	response(nullptr) {
	request->setVersion(Poco::Net::HTTPRequest::HTTP_1_1);
}

EasyRequest::~EasyRequest() {
	if (response) delete response;
	delete request;
}

EasyRequest& EasyRequest::get() {
	request->setMethod(Poco::Net::HTTPRequest::HTTP_GET);
	return *this;
}
EasyRequest& EasyRequest::head() {
	request->setMethod(Poco::Net::HTTPRequest::HTTP_HEAD);
	return *this;
}
EasyRequest& EasyRequest::put() {
	request->setMethod(Poco::Net::HTTPRequest::HTTP_PUT);
	return *this;
}
EasyRequest& EasyRequest::post() {
	request->setMethod(Poco::Net::HTTPRequest::HTTP_POST);
	return *this;
}
EasyRequest& EasyRequest::del() {
	request->setMethod(Poco::Net::HTTPRequest::HTTP_DELETE);
	return *this;
}

EasyRequest& EasyRequest::path(const Poco::URI& uri) {
	std::string path(uri.getPathAndQuery());
	if (path.empty()) {
		path = "/";
	}
	request->setURI(path);
	return *this;
}

EasyRequest& EasyRequest::auth(const Poco::Net::HTTPBasicCredentials& cred) {
	cred.authenticate(*request);
	return *this;
}

EasyRequest& EasyRequest::type(const std::string& type) {
	request->setContentType(type);
	return *this;
}


EasyRequest& EasyRequest::sendContent(json_t* json) {
	abortIfAlreadySent();
	char* jsonData = json_dumps(json, JSON_INDENT(3));
	size_t length = strlen(jsonData);
	sendContent((void*) jsonData, length);
	free(jsonData);
	return *this;
}

EasyRequest& EasyRequest::sendContent(void* data, size_t length) {
	abortIfAlreadySent();
	request->setContentLength(length);
	response = new Poco::Net::HTTPResponse();
	std::ostream& body = session.sendRequest(*request);
	body.write((char*) data, length);
	body.flush();
	return *this;
}

std::istream& EasyRequest::awaitResponse() {
	ensureRequestSent();
	return session.receiveResponse(*response);
}


Poco::Net::HTTPRequest& EasyRequest::getRequest() {
	return *request;
}

Poco::Net::HTTPResponse& EasyRequest::getResponse() {
	return *response;
}

void EasyRequest::abortIfAlreadySent() {
	if (response) {
		throw Exception("Request already sent!");
	}
}

void EasyRequest::ensureRequestSent() {
	if (response == nullptr) {
		response = new Poco::Net::HTTPResponse();
		session.sendRequest(*request);
	}
}

bool EasyRequest::isSuccess() {
	if (!response) return false;
	Poco::Net::HTTPResponse::HTTPStatus status = response->getStatus();
	return (200 <= status) && (status < 300);
}
}
