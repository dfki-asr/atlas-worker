#include <boost/shared_ptr.hpp>
#include <fstream>
#include <boost/utility/empty_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

namespace keywords = boost::log::keywords;
namespace src = boost::log::sources;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;

#pragma once

namespace AssimpWorker {
	
	class BoostLog {
	public:
		BoostLog();
	private:
		void init();
	};
}