#include "abstractDatabaseClient.h"
#include <cache.impl.h>

template DLL_PUBLIC void AdHoc::Cache<IceTray::AbstractDatabaseClient::CacheItem, IceTray::AbstractDatabaseClient::CacheKey>::add(const IceTray::AbstractDatabaseClient::CacheKey &, const IceTray::AbstractDatabaseClient::CacheItem &, time_t);
template DLL_PUBLIC const IceTray::AbstractDatabaseClient::CacheItem * AdHoc::Cache<IceTray::AbstractDatabaseClient::CacheItem, IceTray::AbstractDatabaseClient::CacheKey>::get(const IceTray::AbstractDatabaseClient::CacheKey &) const;

namespace IceTray {
AbstractDatabaseClient::AbstractDatabaseClient(boost::shared_ptr<AdHoc::ResourcePool<DB::Connection>> d) :
	db(d)
{
}

template<>
void
AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const std::string & p)
{
	cmd->bindParamS(o, p);
}

template<>
void
AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const int & p)
{
	cmd->bindParamI(o, p);
}

void
AbstractDatabaseClient::bind(int, DB::Command *)
{
}

void
AbstractDatabaseClient::keyPushParams(CacheKey &)
{
}
}

