#ifndef ICETRAY_ABSTRACTDATABASECLIENT_H
#define ICETRAY_ABSTRACTDATABASECLIENT_H

#include "database.h"
#include "sqlSource.h"
#include <db/sqlSelectDeserializer.h>
#include <slicer/slicer.h>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC AbstractDatabaseClient {
		protected:
			AbstractDatabaseClient(DatabasePoolPtr d);

			template<typename Domain, typename ... Params>
			inline
			Domain
			fetch(const SqlSource & sql, const Params & ... params)
			{
				return fetch<Domain, Params...>(IceUtil::None, sql, params...);
			}

			template<typename Domain, typename ... Params>
			inline
			Domain
			fetch(DB::Connection * c, const SqlSource & sql, const Params & ... params)
			{
				return fetch<Domain, Params...>(c, IceUtil::None, sql, params...);
			}

			template<typename Domain, typename ... Params>
			inline
			Domain
			fetch(const IceUtil::Optional<std::string> & typeIdCol, const SqlSource & sql, const Params & ... params)
			{
				auto c = db->get();
				return fetch<Domain, Params...>(c.get(), typeIdCol, sql, params...);
			}

			template<typename Domain, typename ... Params>
			inline
			Domain
			fetch(DB::Connection * c, const IceUtil::Optional<std::string> & typeIdCol, const SqlSource & sql, const Params & ... params)
			{
				auto s = sql.select(c);
				bind(0, s.get(), params...);
				return Slicer::DeserializeAny<Slicer::SqlSelectDeserializer, Domain>(*s, typeIdCol);
			}

			template<typename Param, typename ... Params>
			static void inline bind(int offset, DB::Command * cmd, const Param & p, const Params & ... params)
			{
				bind1(offset, cmd, p);
				bind(offset + 1, cmd, params...);
			}

			static void bind(int offset, DB::Command * cmd);

			template<typename Param>
			static void bind1(int offset, DB::Command * cmd, const Param & p);

#define BIND1OPTIONAL(T) \
			template<typename Param> \
			static void bind1(int offset, DB::Command * cmd, const T<Param> & p) \
			{ \
				if (p) { \
					bind1(offset, cmd, *p); \
				} \
				else { \
					cmd->bindNull(offset); \
				} \
			}

			BIND1OPTIONAL(IceUtil::Optional);
			BIND1OPTIONAL(boost::optional);
#undef BIND1OPTIONAL

			DatabasePoolPtr db;
	};
}

#endif

