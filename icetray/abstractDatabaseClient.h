#ifndef ICETRAY_ABSTRACTDATABASECLIENT_H
#define ICETRAY_ABSTRACTDATABASECLIENT_H

#include "database.h"
#include <db/sqlSelectDeserializer.h>
#include <slicer/slicer.h>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC AbstractDatabaseClient {
		public:
			AbstractDatabaseClient(DatabasePoolPtr d);

			template<typename Domain, typename Sql, typename ... Params>
			inline
			Domain
			fetch(const Params & ... params)
			{
				auto c = db->get();
				auto s = DB::SelectCommandPtr(c->newSelectCommand(Sql::sql));
				bind(0, s.get(), params...);
				return Slicer::DeserializeAny<Slicer::SqlSelectDeserializer, Domain>(*s);
			}

		private:
			template<typename Param, typename ... Params>
			static void inline bind(int offset, DB::Command * cmd, const Param & p, const Params & ... params)
			{
				bind1(offset, cmd, p);
				bind(offset + 1, cmd, params...);
			}

			static void bind(int offset, DB::Command * cmd);

			template<typename Param>
			static void bind1(int offset, DB::Command * cmd, const Param & p);

			DatabasePoolPtr db;
	};
}

#endif

