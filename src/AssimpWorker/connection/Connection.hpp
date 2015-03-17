/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#pragma warning (push, 3)
#include <activemq/library/ActiveMQCPP.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <cms/Connection.h>
#include <cms/TextMessage.h>
#include <cms/MessageConsumer.h>
#include <cms/MessageProducer.h>
#pragma warning (pop)

namespace AssimpWorker {

	class Connection : public cms::ExceptionListener {

	public:
		Connection();
		Connection(const Connection& other);
		~Connection();

		cms::Session* getSession();
		cms::Connection* getConnection();
		void onException(const cms::CMSException& ex AMQCPP_UNUSED);
		cms::MessageConsumer* getConsumerForQueue(const std::string& queue);
		cms::MessageProducer* getProducerForQueue(const std::string& queue);
		void connect();

	private:
		cms::Connection* connection;
		cms::Session* session;

		cms::Destination* createDestination(const std::string& queue);
	};

}
