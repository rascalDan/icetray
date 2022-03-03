#ifndef ICETRAY_ABSTRACTCACHINGDATABASECLIENT_H
#define ICETRAY_ABSTRACTCACHINGDATABASECLIENT_H

#include "abstractDatabaseClient.h"
#include "sqlSource.h"
#include <any>
#include <cache.h> // IWYU pragma: keep
#include <command.h>
#include <command_fwd.h>
#include <connectionPool.h>
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

namespace IceTray {
	class DLL_PUBLIC AbstractCachingDatabaseClient : public AbstractDatabaseClient {
	private:
		using CacheKey = std::vector<std::size_t>;
		using CacheItem = std::any;

	public:
		explicit AbstractCachingDatabaseClient(const DB::ConnectionPoolPtr & d);

		template<typename Domain, typename... Params>
		inline Domain
		fetchCache(const SqlSource & sql, time_t cacheTime, const Params &... params)
		{
			CacheKey key;
			key.reserve(sizeof...(Params) + 2);
			key.push_back(*sql.getCommandOptions()->hash);
			key.push_back(typeid(Domain).hash_code());
			keyPushParams(key, params...);
			if (auto cached = cache.get(key)) {
				return std::any_cast<Domain>(*cached);
			}
			auto d(fetch<Domain, Params...>(sql, params...));
			cache.add(key, CacheItem(d), time(nullptr) + cacheTime);
			return d;
		}

	private:
		template<typename Param, typename... Params>
		static void inline keyPushParams(CacheKey & k, const Param & p, const Params &... params)
		{
			k.push_back(std::hash<Param>()(p));
			keyPushParams(k, params...);
		}

		static void keyPushParams(CacheKey &);

		using Cache = AdHoc::Cache<CacheItem, CacheKey>;
		Cache cache;
	};
}

#endif
