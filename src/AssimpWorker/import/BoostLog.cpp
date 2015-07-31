#include "BoostLog.hpp"
#include <boost/log/support/date_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace AssimpWorker {
	
	BoostLog::BoostLog() {
		init();
	}

	void BoostLog::init() {

		boost::shared_ptr<logging::core> core = logging::core::get();

		logging::add_common_attributes();
		core->add_global_attribute("Scope", boost::log::attributes::named_scope());

		//setup file log
		logging::add_file_log(
			keywords::file_name = "test.log",
			keywords::format = "[%TimeStamp%]: %Message% in %Scope%"
			);
		// Construct the sink
		typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
		boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

		// console log stream
		sink->locked_backend()->add_stream(
			boost::shared_ptr<std::ostream>(&std::clog, boost::empty_deleter())
			);

		// Register the sink in the logging core
		sink->set_formatter(
			expr::stream
			<< "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S") << "]: "
			<< expr::smessage
			);
		logging::core::get()->add_sink(sink);

	}

	void BoostLog::logSomething(std::string logMessage) {
		src::logger lg;
		BOOST_LOG(lg) << logMessage;
	}

	boost::shared_ptr<BoostLog> BoostLog::getInstance(void) {
		return loggerInstance;
	}

	boost::shared_ptr<BoostLog> BoostLog::loggerInstance(new BoostLog());
}