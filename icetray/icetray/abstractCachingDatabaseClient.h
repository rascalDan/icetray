#ifndef ICETRAY_ABSTRACTCACHINGDATABASECLIENT_H
#define ICETRAY_ABSTRACTCACHINGDATABASECLIENT_H

#include "abstractDatabaseClient.h"
#include "sqlSource.h"
#include <any>
#include <cache.h> // IWYU pragma: keep
#include <command.h>
#include <command_fwd.h>
#include <ctime>
#include <functional>
#include <memory>
#include <optional>
#include <typeinfo>
#include <vector>
#include <visibility.h>
// IWYU pragma: no_include "cache.impl.h"
// IWYU pragma: no_include <boost/multi_index/detail/bidir_node_iterator.hpp>
// IWYU pragma: no_include <boost/operators.hpp>
// IWYU pragma: no_include <variant>

namespace IceTray {
	class DLL_PUBLIC AbstractCachingDatabaseClient : public AbstractDatabaseClient {
	private:
		using CacheKey = std::vector<std::size_t>;
		using CacheItem = std::any;

	public:
		using AbstractDatabaseClient::AbstractDatabaseClient;
		using AbstractDatabaseClient::fetch;
		using AbstractDatabaseClient::modify;

		template<typename Domain, typename... Params>
		inline Domain
		fetchCache(const SqlSource & sql, time_t cacheTime, const Params &... params)
		{
			const CacheKey key {sql.getCommandOptions()->hash.value_or(0), typeid(Domain).hash_code(),
					(std::hash<Params>()(params))...};
			if (auto cached = cache.get(key)) {
				return std::any_cast<Domain>(*cached);
			}
			auto d(fetch<Domain, Params...>(sql, params...));
			cache.add(key, CacheItem(d), time(nullptr) + cacheTime);
			return d;
		}

	private:
		using Cache = AdHoc::Cache<CacheItem, CacheKey>;
		Cache cache;
	};
}

#endif
