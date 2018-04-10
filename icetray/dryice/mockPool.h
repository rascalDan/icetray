#ifndef ICETRAY_MOCKPOOL_H
#define ICETRAY_MOCKPOOL_H

#include <connectionPool.h>
#include <mockDatabase.h>
#include <Ice/Properties.h>

namespace IceTray {
	class MockPool : public DB::BasicConnectionPool {
		public:
			MockPool(const std::string & name, const std::string &, const Ice::PropertiesPtr & p);

			DB::ConnectionPtr createResource() const override;

			const std::string name;
	};
}

#endif

