#ifndef ICETRAY_DATABASE_H
#define ICETRAY_DATABASE_H

#include <connectionPool.h>
#include <boost/shared_ptr.hpp>

namespace IceTray {
	typedef AdHoc::ResourcePool<DB::Connection> DatabasePool;
	typedef boost::shared_ptr<DatabasePool> DatabasePoolPtr;
}

#endif

