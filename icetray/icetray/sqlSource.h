#ifndef ICETRAY_SQLSOURCE_H
#define ICETRAY_SQLSOURCE_H

#include <command_fwd.h>
#include <connection_fwd.h>
#include <string>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC SqlSource {
	public:
		virtual ~SqlSource() = default;

		virtual const std::string & getSql() const = 0;
		virtual DB::CommandOptionsCPtr getCommandOptions() const = 0;

		DB::ModifyCommandPtr modify(DB::Connection * db) const;
		DB::SelectCommandPtr select(DB::Connection * db) const;
	};
}

#endif
