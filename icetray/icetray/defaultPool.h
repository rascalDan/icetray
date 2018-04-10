#ifndef ICETRAY_DEFAULTPOOL_H
#define ICETRAY_DEFAULTPOOL_H

#include <connectionPool.h>
#include <Ice/Properties.h>

namespace IceTray {
	class DefaultPool : public DB::ConnectionPool {
		public:
			DefaultPool(const std::string & name, const std::string & type, const Ice::PropertiesPtr & p);
	};
}

#endif

