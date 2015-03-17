/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <boost/date_time/posix_time/posix_time.hpp>
#include "QueueConsumer.hpp"
#include "../internal/configuration.hpp"
#include "../worker/Worker.hpp"
#include "../worker/WorkUnit.hpp"

namespace AssimpWorker {

	QueueConsumer::QueueConsumer() :
		mutex(),
		connection(),
		consumer(NULL),
		heartbeatProducer(NULL)
	{
	}

	QueueConsumer::QueueConsumer(const QueueConsumer& other) :
		mutex(),
		connection(other.connection),
		consumer(other.consumer),
		heartbeatProducer(other.heartbeatProducer)
	{
	}

	QueueConsumer::~QueueConsumer() {
		cleanup();
	}

	// entry point for boost::thread
	void QueueConsumer::operator()() {
        try {
            connection.connect();
        } catch (cms::CMSException& ex) {
            std::cerr << "Could not connect to work queue: " << ex.getMessage() << std::endl;
            std::cerr << "Check if the server is up, and the STOMP parameters are set correctly." << std::endl;
            std::cerr << "See --help for details." << std::endl;
            return;
        }
        std::cout << "STOMP connection established, waiting for work orders." << std::endl;
		consumer = connection.getConsumerForQueue("work-queue");
		consumer->setMessageListener(this);
		heartbeatProducer = connection.getProducerForQueue("work-queue");
		startHeartbeatLoop();
	}

	void QueueConsumer::startHeartbeatLoop() {
		std::unique_ptr<cms::TextMessage> heartbeat(connection.getSession()->createTextMessage());
		heartbeat->setText("ping");
		Configuration& config = Configuration::getInstance();
		int heartbeatInterval = config.get("stomp-heartbeat-interval-ms").as<int>();
#pragma warning(disable: 4127) // warning C4127 : conditional expression is constant
		while (true) {
			heartbeatProducer->send(heartbeat.get());
			boost::this_thread::sleep(boost::posix_time::milliseconds(heartbeatInterval));
		}
	}

	// Called from the consumer since this class is a registered MessageListener.
	void QueueConsumer::onMessage(const cms::Message* message) {
		try {
			mutex.lock(); // make sure we're not handling two messages at the same time
			const cms::TextMessage* textMessage = dynamic_cast<const cms::TextMessage*> (message);
			parseMessage(textMessage);
		}
		catch (cms::CMSException& e) {
			printf("Exception caught in onMessage! ");
			e.printStackTrace();
		}
		mutex.unlock(); // now other messages can be processed.
	}

	void QueueConsumer::parseMessage(const cms::TextMessage* message) {
		std::string text = message->getText();
		if (text == "import") {
			const std::string filePath = message->getStringProperty("filePath");
			const std::string importOperationId = message->getStringProperty("importOperationId");
			const std::string assetName = message->getStringProperty("assetName");
			const std::string fileType = message->getStringProperty("fileType");
			printf("Received import message with filepath %s\n", filePath.c_str());
			startImportWorker(filePath, importOperationId, assetName, fileType);
		}
	}

	void QueueConsumer::startImportWorker(const std::string& filePath, const std::string& importOperationId, const std::string& assetName, const std::string& fileType) {
		WorkUnit workUnit("import", importOperationId, assetName);
		workUnit.fileType = fileType;
		workUnit.sourcePath = filePath;
		Worker worker;
		worker.import(workUnit);
	}

	void QueueConsumer::cleanup() {
		// Destroy resources.
		try {
			delete consumer;
			consumer = NULL;
			delete heartbeatProducer;
			heartbeatProducer = NULL;
		}
		catch (cms::CMSException& e) {
			e.printStackTrace();
		}
	}
}
