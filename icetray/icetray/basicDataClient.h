#ifndef ICETRAY_BASICDATABASECLIENT_H
#define ICETRAY_BASICDATABASECLIENT_H

#include "sqlSource.h"
#include <Ice/BuiltinSequences.h>
#include <command.h>
#include <db/sqlSelectDeserializer.h> // IWYU pragma: keep
#include <memory>
#include <modifycommand.h> // IWYU pragma: keep
#include <optional>
#include <selectcommand.h> // IWYU pragma: keep
#include <slicer/slicer.h>
#include <string>
#include <visibility.h>
namespace DB {
	class Connection;
}

namespace IceTray {
	class DLL_PUBLIC BasicDatabaseClient {
	protected:
		template<typename Domain, typename... Params>
		static inline Domain
		fetch(DB::Connection * c, const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(c, std::nullopt, sql, params...);
		}

		template<typename Domain, typename... Params>
		static inline Domain
		fetch(DB::Connection * c, const std::optional<std::string> & typeIdCol, const SqlSource & sql,
				const Params &... params)
		{
			auto s = sql.select(c);
			bind(s.get(), params...);
			return Slicer::DeserializeAny<Slicer::SqlSelectDeserializer, Domain>(s.get(), typeIdCol);
		}

		template<typename... Params>
		static inline auto
		modify(DB::Connection * c, const SqlSource & sql, const Params &... params)
		{
			auto s = sql.modify(c);
			bind(s.get(), params...);
			return s->execute();
		}

	protected:
		template<typename... Params> static void inline bind(DB::Command * cmd, const Params &... params)
		{
			unsigned int offset {};
			(cmd->bindParam(offset++, params), ...);
		}
	};
}

namespace DB {
	template<> void Command::bindParam(unsigned int offset, const Ice::ByteSeq & p);
}

#endif
