#include "abstractCachingDatabaseClient.h"
#include <cache.impl.h>

template class AdHoc::Cache<IceTray::AbstractCachingDatabaseClient::CacheItem,
		IceTray::AbstractCachingDatabaseClient::CacheKey>;
