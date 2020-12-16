#ifndef ICETRAY_SQLSOURCE_H
#define ICETRAY_SQLSOURCE_H

#include <c++11Helpers.h>
#include <command_fwd.h>
#include <connection_fwd.h>
#include <string>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC SqlSource {
	public:
		SqlSource() = default;
		virtual ~SqlSource() = default;

		SPECIAL_MEMBERS_DELETE(SqlSource);

		[[nodiscard]] virtual const std::string & getSql() const = 0;
		[[nodiscard]] virtual DB::CommandOptionsCPtr getCommandOptions() const = 0;

		DB::ModifyCommandPtr modify(DB::Connection * db) const;
		DB::SelectCommandPtr select(DB::Connection * db) const;
	};
}

#endif
