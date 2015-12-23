#ifndef ICETRAY_MOCKPOOL_H
#define ICETRAY_MOCKPOOL_H

#include <connectionPool.h>
#include <mockDatabase.h>
#include <Ice/Properties.h>

namespace IceTray {
	class MockPool : public AdHoc::ResourcePool<DB::Connection> {
		public:
			MockPool(const std::string & name, const std::string &, Ice::PropertiesPtr p);

			DB::Connection * createResource() const override;

			const std::string name;
	};
}

#endif

