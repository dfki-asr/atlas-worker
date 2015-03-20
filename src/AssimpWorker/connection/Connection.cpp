/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include "Connection.hpp"
#include "../internal/configuration.hpp"
#include <sstream>

namespace AssimpWorker {

	Connection::Connection() :
		connection(NULL),
		session(NULL)
	{
	}

	Connection::Connection(const Connection& other) :
		connection(other.connection),
		session(other.session)
	{
	}

	Connection::~Connection() {
		if (connection != NULL) {
			try {
				connection->close();
				delete connection;
				connection = NULL;
				delete session;
				session = NULL;
			}
			catch (cms::CMSException& ex) {
				ex.printStackTrace();
			}
		}
	}

	cms::Session* Connection::getSession() {
		return session;
	}

	cms::Connection* Connection::getConnection() {
		return connection;
	}

	std::string Connection::getBrokerUrl() {
		Configuration& config = Configuration::getInstance();
		std::stringstream url;
		url << "tcp://" 
		    << config.get("stomp-host").as<std::string>()
		    << ":"
		    << config.get("stomp-port").as<int>()
		    << "?wireFormat=stomp"
		    << "&wireFormat.queuePrefix=jms.queue."
		    << "&connection.watchTopicAdvisories=false";
		return url.str();
	}

	void Connection::connect() {
		Configuration& config = Configuration::getInstance();
		std::auto_ptr<cms::ConnectionFactory> connectionFactory(
			cms::ConnectionFactory::createCMSConnectionFactory(getBrokerUrl())
		);
		connection = connectionFactory->createConnection(
			config.get("stomp-user").as<std::string>(),
			config.get("stomp-pass").as<std::string>()
		);
		try {
		connection->start();
		} catch (const cms::CMSException& e) {
			std::cerr << "Connection error: " << e.getMessage() << std::endl;
			return;
		}
		connection->setExceptionListener(this);
		session = connection->createSession(cms::Session::AUTO_ACKNOWLEDGE);
	}

	void Connection::onException(const cms::CMSException& ex AMQCPP_UNUSED) {
		printf("CMS Exception occurred.  Shutting down client.\n");
		ex.printStackTrace();
		exit(1);
	}

	cms::MessageConsumer* Connection::getConsumerForQueue(const std::string& queue) {
		return session->createConsumer(createDestination(queue));
	}

	cms::MessageProducer* Connection::getProducerForQueue(const std::string& queue) {
		return session->createProducer(createDestination(queue));
	}

	cms::Destination* Connection::createDestination(const std::string& queue) {
		Configuration& config = Configuration::getInstance();
		return session->createQueue(config.get(queue).as<std::string>());
	}
}
