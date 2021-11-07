#include "mockPool.h"
#include <Ice/PropertiesF.h>
#include <connectionPool.h>
#include <connection_fwd.h>
#include <factory.impl.h>
#include <icetrayService.h>
#include <memory>
#include <mockDatabase.h>
#include <utility>

namespace IceTray {
	MockPool::MockPool(const std::string & name, const std::string &, const Ice::PropertiesPtr & p) :
		MockPool(name, static_cast<unsigned int>(p->getPropertyAsIntWithDefault(name + ".Database.PoolMax", 10)),
				static_cast<unsigned int>(p->getPropertyAsIntWithDefault(name + ".Database.PoolKeep", 2)))
	{
	}

	MockPool::MockPool(std::string name, unsigned int size, unsigned int keep) :
		DB::BasicConnectionPool(size, keep), name(std::move(name))
	{
	}

	DB::ConnectionPtr
	MockPool::createResource() const
	{
		return DB::MockDatabase::openConnectionTo(name);
	}

	FACTORY(MockPool, PoolProvider)
}
