#ifndef ICETRAY_STATICSQLSOURCE_H
#define ICETRAY_STATICSQLSOURCE_H

#include "sqlSource.h"

namespace IceTray {
	class DLL_PUBLIC StaticSqlSource : public SqlSource {
		public:
			StaticSqlSource(const std::string & sql);
			StaticSqlSource(const std::string & sql, const std::string & optsName, const DB::CommandOptionsMap &);

			const std::string & getSql() const override;
			DB::CommandOptionsCPtr getCommandOptions() const override;

		protected:
			const std::string sql;
			const DB::CommandOptionsCPtr opts;
	};
}

#endif

