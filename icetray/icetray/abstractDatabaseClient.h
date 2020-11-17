#ifndef ICETRAY_ABSTRACTDATABASECLIENT_H
#define ICETRAY_ABSTRACTDATABASECLIENT_H

#include "sqlSource.h"
#include <Ice/BuiltinSequences.h>
#include <connectionPool.h>
#include <db/sqlSelectDeserializer.h>
#include <slicer/slicer.h>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC AbstractDatabaseClient {
	protected:
		AbstractDatabaseClient(DB::ConnectionPoolPtr d);

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(IceUtil::None, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(DB::Connection * c, const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(c, IceUtil::None, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const IceUtil::Optional<std::string> & typeIdCol, const SqlSource & sql, const Params &... params)
		{
			auto c = db->get();
			return fetch<Domain, Params...>(c.get(), typeIdCol, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(DB::Connection * c, const IceUtil::Optional<std::string> & typeIdCol, const SqlSource & sql,
				const Params &... params)
		{
			auto s = sql.select(c);
			bind(0, s.get(), params...);
			return Slicer::DeserializeAny<Slicer::SqlSelectDeserializer, Domain>(s.get(), typeIdCol);
		}

		template<typename Param, typename... Params>
		static void inline bind(unsigned int offset, DB::Command * cmd, const Param & p, const Params &... params)
		{
			cmd->bindParam(offset, p);
			bind(offset + 1, cmd, params...);
		}

		static void inline bind(int, DB::Command *) { }

		const DB::ConnectionPoolPtr db;
	};
}

namespace DB {
	template<> void Command::bindParam(unsigned int offset, const Ice::ByteSeq & p);
}

#endif
