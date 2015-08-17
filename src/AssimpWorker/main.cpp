/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <iostream>
#include <csignal>
#include "internal/configuration.hpp"
#include <activemq/library/ActiveMQCPP.h>
#include <boost/thread.hpp>
#include "connection/QueueConsumer.hpp"
#include "internal/AssimpVersion.hpp"

boost::thread* g_QueueConsumerThread = nullptr;

void showAtlasLogo() {
	printf("\n"
	"  ______      ______      __          ______      ____\n"
	" /\\  _  \\    /\\__  _\\    /\\ \\        /\\  _  \\    /\\  _`\n"
	" \\ \\ \\L\\ \\   \\/_/\\ \\/    \\ \\ \\       \\ \\ \\L\\ \\   \\ \\,\\L\\_\\\n"
	"  \\ \\  __ \\     \\ \\ \\     \\ \\ \\  __   \\ \\  __ \\   \\/_\\__ \\\n"
	"   \\ \\ \\/\\ \\     \\ \\ \\     \\ \\ \\L\\ \\   \\ \\ \\/\\ \\    /\\ \\L\\ \\\n"
	"    \\ \\_\\ \\_\\     \\ \\_\\     \\ \\____/    \\ \\_\\ \\_\\   \\ `\\____\\\n"
	"     \\/_/\\/_/      \\/_/      \\/___/      \\/_/\\/_/    \\/_____/\n"
	" Advanced # Three-dimensional # Large-scale # Asset # Server \n"
	"\n");
}

void signalHandler(int signalNumber) {
	std::cout << "Termination signal caught. Disconnecting from Work queue." << std::endl;
	if (g_QueueConsumerThread) {
		g_QueueConsumerThread->interrupt();
		g_QueueConsumerThread->join();
	}
}

int main(int argc, char** argv)
{
	AssimpWorker::Configuration& config = AssimpWorker::Configuration::getInstance();
	config.init(argc, argv);

	showAtlasLogo();
	std::cout << "Running on " << AssimpWorker::AssimpVersion::getAssimpVersion() << std::endl;

	activemq::library::ActiveMQCPP::initializeLibrary();
	AssimpWorker::QueueConsumer consumer;
	boost::thread consumerThread(consumer);
	g_QueueConsumerThread = &consumerThread;

	std::cout << "Establishing connection in background thread." << std::endl;
	std::signal(SIGTERM, &signalHandler);
	std::signal(SIGINT, &signalHandler);

	// wait for consumer thread to finish.
	g_QueueConsumerThread->join();
	activemq::library::ActiveMQCPP::shutdownLibrary();

	return 0;
}
