#ifndef ICETRAY_MOCKPOOL_H
#define ICETRAY_MOCKPOOL_H

#include <Ice/Properties.h>
#include <connectionPool.h>
#include <connection_fwd.h>
#include <string>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC MockPool : public DB::BasicConnectionPool {
	public:
		MockPool(const std::string & name, const std::string &, const Ice::PropertiesPtr & p);
		MockPool(std::string name, unsigned int size, unsigned int keep);

		DB::ConnectionPtr createResource() const override;

		const std::string name;
	};
}

#endif
