#ifndef ICETRAY_STATICSQLSOURCE_H
#define ICETRAY_STATICSQLSOURCE_H

#include "sqlSource.h"
#include <command_fwd.h>
#include <string>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC StaticSqlSource : public SqlSource {
	public:
		explicit StaticSqlSource(std::string sql);
		StaticSqlSource(std::string sql, const std::string & optsName, const DB::CommandOptionsMap &);

		[[nodiscard]] const std::string & getSql() const override;
		[[nodiscard]] DB::CommandOptionsCPtr getCommandOptions() const override;

	protected:
		const std::string sql;
		const DB::CommandOptionsCPtr opts;
	};
}

#endif
