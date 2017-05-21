#ifndef ICETRAY_SQLSOURCE_H
#define ICETRAY_SQLSOURCE_H

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
			virtual const DB::CommandOptions * getCommandOptions() const = 0;

			DB::ModifyCommandPtr modify(DB::Connection * db) const;
			DB::SelectCommandPtr select(DB::Connection * db) const;
	};
}

#endif

