/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#pragma warning (push, 3)
#include <activemq/library/ActiveMQCPP.h>
#include <cms/MessageProducer.h>
#include <cms/TextMessage.h>
#pragma warning (pop)
#include "../worker/WorkUnit.hpp"
#include "../connection/Connection.hpp"


namespace AssimpWorker {

	class FeedbackProducer {
	
	public:
		FeedbackProducer();
		FeedbackProducer(const FeedbackProducer& other) = delete;
		~FeedbackProducer();

		void sendWorkFeedbackMessage(WorkUnit& workUnit, const std::string& status);

	private:
		Connection connection;
		cms::MessageProducer* producer;

		void cleanup();
		std::unique_ptr<cms::TextMessage> createMessageFromWorkUnit(WorkUnit& workUnit, const std::string& status);
	};

}
