#include "mockPool.h"
#include "icetrayService.h"
#include <factory.impl.h>

namespace IceTray {
	MockPool::MockPool(const std::string & name, const std::string &, Ice::PropertiesPtr p) :
		AdHoc::ResourcePool<DB::Connection>(
					p->getPropertyAsIntWithDefault(name + ".Database.PoolMax", 10),
					p->getPropertyAsIntWithDefault(name + ".Database.PoolKeep", 2)),
		name(name)
	{
	}

	DB::Connection * MockPool::createResource() const
	{
		return DB::MockDatabase::openConnectionTo(name);
	}

	FACTORY(MockPool, PoolProvider);
};

