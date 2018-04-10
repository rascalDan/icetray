#ifndef ICETRAY_DATABASE_H
#define ICETRAY_DATABASE_H

#include <connectionPool.h>
#include <memory>

namespace IceTray {
	typedef AdHoc::ResourcePool<DB::Connection> DatabasePool;
	typedef std::shared_ptr<DatabasePool> DatabasePoolPtr;
}

#endif

