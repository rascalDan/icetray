#ifndef ICETRAY_MOCKPOOL_H
#define ICETRAY_MOCKPOOL_H

#include "database.h"
#include <mockDatabase.h>
#include <Ice/Properties.h>

namespace IceTray {
	class MockPool : public DatabasePool {
		public:
			MockPool(const std::string & name, const std::string &, Ice::PropertiesPtr p);

			DB::Connection * createResource() const override;

			const std::string name;
	};
}

#endif

