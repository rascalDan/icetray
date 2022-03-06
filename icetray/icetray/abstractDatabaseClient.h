#ifndef ICETRAY_ABSTRACTDATABASECLIENT_H
#define ICETRAY_ABSTRACTDATABASECLIENT_H

#include "basicDataClient.h"
#include <connectionPool.h>
#include <memory>
#include <optional>
#include <string>
#include <visibility.h>
// IWYU pragma: no_include "resourcePool.impl.h"

namespace IceTray {
	class SqlSource;

	class DLL_PUBLIC AbstractDatabaseClient : public BasicDatabaseClient {
	public:
		explicit AbstractDatabaseClient(DB::ConnectionPoolPtr d);

		using BasicDatabaseClient::fetch;
		using BasicDatabaseClient::modify;

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const SqlSource & sql, const Params &... params)
		{
			return fetch<Domain, Params...>(std::nullopt, sql, params...);
		}

		template<typename Domain, typename... Params>
		inline Domain
		fetch(const std::optional<std::string> & typeIdCol, const SqlSource & sql, const Params &... params)
		{
			auto c = db->get();
			return fetch<Domain, Params...>(c.get(), typeIdCol, sql, params...);
		}

		template<typename... Params>
		inline auto
		modify(const SqlSource & sql, const Params &... params)
		{
			auto c = db->get();
			return modify(c.get(), sql, params...);
		}

	protected:
		DB::ConnectionPoolPtr db;
	};
}

#endif
