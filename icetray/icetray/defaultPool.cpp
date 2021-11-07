#include "defaultPool.h"
#include "icetrayService.h"
#include <Ice/Properties.h>
#include <Ice/PropertiesF.h>
#include <connectionPool.h>
#include <factory.h>
#include <memory>
#include <string>

namespace IceTray {
	DefaultPool::DefaultPool(const std::string & name, const std::string & type, const Ice::PropertiesPtr & p) :
		DB::ConnectionPool(static_cast<unsigned int>(p->getPropertyAsIntWithDefault(name + ".Database.PoolMax", 10)),
				static_cast<unsigned int>(p->getPropertyAsIntWithDefault(name + ".Database.PoolKeep", 2)),
				p->getPropertyWithDefault(name + ".Database.Type", type),
				p->getProperty(name + ".Database.ConnectionString"))
	{
	}

	FACTORY(DefaultPool, PoolProvider)
}
