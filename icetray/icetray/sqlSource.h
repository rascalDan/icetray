#ifndef ICETRAY_SQLSOURCE_H
#define ICETRAY_SQLSOURCE_H

#include <functional>
#include <string>
#include <visibility.h>
#include <connection.h>
#include <modifycommand.h>
#include <selectcommand.h>

namespace IceTray {
	class DLL_PUBLIC SqlSource {
		public:
			virtual ~SqlSource() = default;

			virtual const std::string & getSql() const = 0;
			virtual std::size_t getSqlHash() const;

			template<typename OptsType = DB::CommandOptions, typename ... Opts>
			DB::ModifyCommandPtr modify(DB::Connection * db, const Opts & ... opts) const
			{
				OptsType o(getSqlHash(), opts...);
				return db->modify(getSql(), &o);
			}

			template<typename OptsType = DB::CommandOptions, typename ... Opts>
			DB::SelectCommandPtr select(DB::Connection * db, const Opts & ... opts) const
			{
				OptsType o(getSqlHash(), opts...);
				return db->select(getSql(), &o);
			}
	};
}

#endif

