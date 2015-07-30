#include "BoostLog.hpp"
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