/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#pragma warning (push, 3)
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/TextMessage.h>
#pragma warning (pop)
#include <boost/thread.hpp>
#include "../connection/Connection.hpp"

namespace AssimpWorker {

	class QueueConsumer : public cms::MessageListener {
	
	public:
		QueueConsumer();
		QueueConsumer(const QueueConsumer& other);
		~QueueConsumer();

		void operator()();
		void onMessage(const cms::Message* message);
		void cleanup();

	private:
		boost::mutex mutex;
		Connection connection;
		cms::MessageConsumer* consumer;
		cms::MessageProducer* heartbeatProducer;

		void parseMessage(const cms::TextMessage* message);
		void startImportWorker(const std::string& filePath, const std::string& importOperationId, const std::string& assetName, const std::string& fileType);
		void startHeartbeatLoop();
	};

}
