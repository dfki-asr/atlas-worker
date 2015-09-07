/*
* This file is part of ATLAS. It is subject to the license terms in
* the LICENSE file found in the top-level directory of this distribution.
* (Also avialable at http://www.apache.org/licenses/LICENSE-2.0.txt)
* You may not use this file except in compliance with the License.
*/
#pragma once

#include <string>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

namespace AssimpWorker {

class Configuration
{
	public:
		static Configuration& getInstance();
		void init(int argc, char** argv);
		// get() is very read-only, you should be able to call it safely from anywhere.
		const boost::program_options::variable_value& get(const std::string& entry) const;
		const bool enabled(const std::string& entry) const;
		void printDescription() const;
	private:
		Configuration();
		void setupOptions();
		void parseConfigFile();
		bool defaultConfigExists();

		static const char* defaultConfigFilename;
		static Configuration* singletonInstance;
		boost::program_options::variables_map parsedVariables;
		boost::program_options::options_description description;
};

}
