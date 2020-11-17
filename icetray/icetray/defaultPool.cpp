#include "defaultPool.h"
#include "icetrayService.h"

namespace IceTray {
	DefaultPool::DefaultPool(const std::string & name, const std::string & type, const Ice::PropertiesPtr & p) :
		DB::ConnectionPool(p->getPropertyAsIntWithDefault(name + ".Database.PoolMax", 10),
				p->getPropertyAsIntWithDefault(name + ".Database.PoolKeep", 2),
				p->getPropertyWithDefault(name + ".Database.Type", type),
				p->getProperty(name + ".Database.ConnectionString"))
	{
	}

	FACTORY(DefaultPool, PoolProvider);
}
