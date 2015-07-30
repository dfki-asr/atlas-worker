#include <fstream>
#include "BoostLog.hpp"
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>


namespace keywords = boost::log::keywords;
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

		// console log stream
		sink->locked_backend()->add_stream(
			boost::shared_ptr<std::ostream>(&std::clog, boost::empty_deleter())
			);
		//file log stream
		sink->locked_backend()->add_stream(
			boost::make_shared< std::ofstream >("sample.log"));
		// Register the sink in the logging core
		logging::core::get()->add_sink(sink);
	}
}