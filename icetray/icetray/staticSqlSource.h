#ifndef ICETRAY_STATICSQLSOURCE_H
#define ICETRAY_STATICSQLSOURCE_H

#include "sqlSource.h"

namespace IceTray {
	class DLL_PUBLIC StaticSqlSource : public SqlSource {
		public:
			StaticSqlSource(const std::string &);

			const std::string & getSql() const override;
			std::size_t getSqlHash() const override;

		protected:
			const std::string sql;
			const std::size_t hash;
	};
}

#endif


