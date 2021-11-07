#ifndef ICETRAY_ABSTRACTDATABASECLIENT_H
#define ICETRAY_ABSTRACTDATABASECLIENT_H

#include "sqlSource.h"
#include <Ice/BuiltinSequences.h>
#include <command.h>
#include <connectionPool.h>
#include <db/sqlSelectDeserializer.h> // IWYU pragma: keep
#include <memory>
#include <optional>
#include <resourcePool.impl.h>
#include <selectcommand.h> // IWYU pragma: keep
#include <slicer/slicer.h>
#include <string>
#include <visibility.h>
namespace DB {
	class Connection;
}

namespace IceTray {
	class DLL_PUBLIC AbstractDatabaseClient {
	protected:
		explicit AbstractDatabaseClient(DB::ConnectionPoolPtr d);

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(std::nullopt, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(DB::Connection * c, const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(c, std::nullopt, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const std::optional<std::string> & typeIdCol, const SqlSource & sql, const Params &... params)
		{
			auto c = db->get();
			return fetch<Domain, Params...>(c.get(), typeIdCol, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(DB::Connection * c, const std::optional<std::string> & typeIdCol, const SqlSource & sql,
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

		static void inline bind(unsigned int, DB::Command *) { }

		DB::ConnectionPoolPtr db;
	};
}

namespace DB {
	template<> void Command::bindParam(unsigned int offset, const Ice::ByteSeq & p);
}

#endif
