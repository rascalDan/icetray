#ifndef ICETRAY_ABSTRACTCACHINGDATABASECLIENT_H
#define ICETRAY_ABSTRACTCACHINGDATABASECLIENT_H

#include "abstractDatabaseClient.h"
#include <cache.h>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>

namespace IceTray {
	class DLL_PUBLIC AbstractCachingDatabaseClient : public AbstractDatabaseClient {
		private:
			typedef std::vector<std::size_t> CacheKey;
			typedef boost::any CacheItem;

		protected:
			AbstractCachingDatabaseClient(DatabasePoolPtr d);

			template<typename Domain, typename Sql, typename ... Params>
			inline
			Domain
			fetchCache(time_t cacheTime, const Params & ... params)
			{
				CacheKey key;
				key.reserve(sizeof...(Params) + 2);
				key.push_back(Sql::hash);
				key.push_back(typeid(Domain).hash_code());
				keyPushParams(key, params...);
				if (auto cached = cache.get(key)) {
					return boost::any_cast<Domain>(*cached);
				}
				auto d(fetch<Domain, Sql, Params...>(params...));
				cache.add(key, CacheItem(d), time(NULL) + cacheTime);
				return d;
			}

		private:
			template<typename Param, typename ... Params>
			static void inline keyPushParams(CacheKey & k, const Param & p, const Params & ... params)
			{
				k.push_back(std::hash<Param>()(p));
				keyPushParams(k, params...);
			}

			static void keyPushParams(CacheKey &);

			typedef AdHoc::Cache<CacheItem, CacheKey> Cache;
			Cache cache;
	};
}

#endif

