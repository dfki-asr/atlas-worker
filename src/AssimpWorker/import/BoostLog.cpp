#include "BoostLog.hpp"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;

namespace AssimpWorker {
	
	BoostLog::BoostLog() {
		init();
	}

	void BoostLog::init() {

		boost::shared_ptr<logging::core> core = logging::core::get();

		//setup file log
		logging::add_file_log(
			keywords::file_name = "test.log",
			keywords::format = "[%Timestamp]"
			);
		// Construct the sink
		typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
		boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

		// Register the sink in the logging core
		logging::core::get()->add_sink(sink);
	}
}