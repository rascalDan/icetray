#include "abstractCachingDatabaseClient.h"
#include <cache.impl.h>

template DLL_PUBLIC void
AdHoc::Cache<IceTray::AbstractCachingDatabaseClient::CacheItem, IceTray::AbstractCachingDatabaseClient::CacheKey>::add(
		const IceTray::AbstractCachingDatabaseClient::CacheKey &,
		const IceTray::AbstractCachingDatabaseClient::CacheItem &, time_t);
template DLL_PUBLIC IceTray::AbstractCachingDatabaseClient::Cache::Value
AdHoc::Cache<IceTray::AbstractCachingDatabaseClient::CacheItem, IceTray::AbstractCachingDatabaseClient::CacheKey>::get(
		const IceTray::AbstractCachingDatabaseClient::CacheKey &) const;

namespace IceTray {
	AbstractCachingDatabaseClient::AbstractCachingDatabaseClient(const DB::ConnectionPoolPtr & d) :
		AbstractDatabaseClient(d)
	{
	}

	void
	AbstractCachingDatabaseClient::keyPushParams(CacheKey &)
	{
	}
}
