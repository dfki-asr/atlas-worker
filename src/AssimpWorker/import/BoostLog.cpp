#include "BoostLog.hpp"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;

namespace AssimpWorker {
	
	BoostLog::BoostLog() {
		init();
	}

	void BoostLog::init() {
		logging::add_file_log("test.log");
		logging::core::get()->set_filter(
			logging::trivial::severity >= logging::trivial::info
			);

	}
}