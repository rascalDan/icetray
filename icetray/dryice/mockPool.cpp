#include "mockPool.h"
#include "icetrayService.h"
#include <factory.impl.h>

namespace IceTray {
	MockPool::MockPool(const std::string & name, const std::string &, const Ice::PropertiesPtr & p) :
		DatabasePool(
					p->getPropertyAsIntWithDefault(name + ".Database.PoolMax", 10),
					p->getPropertyAsIntWithDefault(name + ".Database.PoolKeep", 2)),
		name(name)
	{
	}

	DB::ConnectionPtr MockPool::createResource() const
	{
		return DB::MockDatabase::openConnectionTo(name);
	}

	FACTORY(MockPool, PoolProvider);
};

