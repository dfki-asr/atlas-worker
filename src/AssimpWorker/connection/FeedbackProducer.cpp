/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <algorithm>
#include "../internal/configuration.hpp"
#include "FeedbackProducer.hpp"

namespace AssimpWorker {

	FeedbackProducer::FeedbackProducer() :
		connection(),
		producer(nullptr)
	{
	}

	FeedbackProducer::~FeedbackProducer() {
		cleanup();
	}

	void FeedbackProducer::sendWorkFeedbackMessage(WorkUnit& workUnit, const std::string& status) {
		if (producer == nullptr) {
			connection.connect();
			producer = connection.getProducerForQueue("feedback-queue");
		}
		std::unique_ptr<cms::TextMessage> msg = createMessageFromWorkUnit(workUnit, status);
		producer->send(msg.get());
	}

	std::unique_ptr<cms::TextMessage> FeedbackProducer::createMessageFromWorkUnit(WorkUnit& workUnit, const std::string& status) {
		std::unique_ptr<cms::TextMessage> msg(connection.getSession()->createTextMessage());
		msg->setText(workUnit.workType);
		msg->setStringProperty("status", status);
		msg->setStringProperty("importOperationId", workUnit.importOperationId);
		msg->setStringProperty("resultPath", workUnit.resultPath);
		std::string safeDetail(workUnit.detail);
		//@Hack: Newlines in text message properties break STOMP. The choice here was to move to MapMessage (and OpenWire) or "clean"
		//		 the detail string (which can hold n error messages) for transport to the JBOSS server. Maybe there's a better way to handle this?
		std::replace(safeDetail.begin(), safeDetail.end(), '\n', '^');
		msg->setStringProperty("detail", safeDetail);
		msg->setLongProperty("timeTaken", (long) difftime(workUnit.startTime, workUnit.endTime));
		return msg;
	}

	void FeedbackProducer::cleanup() {
		// Destroy resources.
		try {
			delete producer;
			producer = nullptr;
		}
		catch (cms::CMSException& e) {
			e.printStackTrace();
		}
	}
}
