/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#include <fstream>
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>
#include "configuration.hpp"

namespace bpo = boost::program_options;

namespace AssimpWorker {

Configuration* Configuration::singletonInstance = NULL;
const char* Configuration::defaultConfigFilename = "assimpworker.conf";

Configuration& Configuration::getInstance() {
	if (singletonInstance == NULL) {
		singletonInstance = new Configuration();
	}
	return *singletonInstance;
}

Configuration::Configuration()
{
	setupOptions();
	parseConfigFile();
}

void Configuration::init(int argc, char **argv) {
	bpo::command_line_parser parser(argc, argv);
	parser.options(description);
	bpo::store(parser.run(),parsedVariables);
	// in case the default file didn't exist
	// and we now know a non-default name:
	parseConfigFile();
	if (parsedVariables.count("help")) {
		std::cout << description << std::endl;
		exit(1);
	}
	// now that we have parsed commandline
	// as well as default and possibly other configfile
	// check for missing stuff:
	try {
		bpo::notify(parsedVariables);
	} catch (std::exception const& e) {
		std::cerr << "Parsing options:" << e.what() << std::endl;
		exit(1);
	}
}

void Configuration::parseConfigFile() {
	const char* configFileName = NULL;
	if (parsedVariables.count("config")) {
		// config file specified
		std::string configFile = parsedVariables["config"].as<std::string>();
		configFileName = configFile.c_str();
	} else if (defaultConfigExists()) {
		configFileName = defaultConfigFilename;
	}
	if (!configFileName){
		return;
	}
	std::ifstream configFile(configFileName);
	if (configFile.good()) {
		bpo::parsed_options parsedConfig = bpo::parse_config_file(configFile,description);
		bpo::store(parsedConfig, parsedVariables);
	}
	// don't notify here, as we may not have parsed commandline yet.
}

bool Configuration::defaultConfigExists() {
	return boost::filesystem::exists(defaultConfigFilename);
}

void Configuration::setupOptions() {
	bpo::options_description basic("Basic options");
	basic.add_options()
		("help,h,?", "show this message")
		("version,v","show version number")
		("config,c", bpo::value<std::string>(), "read configuration from file, overrides options given on command line")
		("decompression-path", bpo::value<std::string>()->default_value("./tmp"), "Path to temporary space for decompressed zip contents")
	;
	bpo::options_description stomp("Stomp options");
	stomp.add_options()
		("stomp-heartbeat-interval-ms", bpo::value<int>()->default_value(10000), "Heartbeat interval in ms")
		("stomp-port", bpo::value<int>()        ->default_value(61613),              "Server port (STOMP protocol)")
		("stomp-host", bpo::value<std::string>()->default_value("localhost"),        "Server address")
		("stomp-user", bpo::value<std::string>()->required(),                        "Username")
		("stomp-pass", bpo::value<std::string>()->required(),                        "Password")
		("work-queue",bpo::value<std::string>()->default_value("atlas.work.import"),"Work queue name")
		("feedback-queue",bpo::value<std::string>()->default_value("atlas.work.feedback"),"Feedback queue name")
	;
	bpo::options_description jcr("Repository options");
	jcr.add_options()
		("jcr-url",
		 bpo::value<std::string>()->default_value("http://localhost:8080/modeshape-rest/atlas/default/"),
		 "Root URL")
		("jcr-user",
		 bpo::value<std::string>()->default_value("admin"),
		 "Username")
		("jcr-pass",
		 bpo::value<std::string>()->default_value("admin"),
		 "Password")
	;

	description.add(basic);
	description.add(stomp);
	description.add(jcr);
}

const bpo::variable_value& Configuration::get(const std::string &entry) const {
	return parsedVariables[entry];
}

const bool Configuration::enabled(const std::string& entry) const
{
	return parsedVariables.count(entry);
}

}

