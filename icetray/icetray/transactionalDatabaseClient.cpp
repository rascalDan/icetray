#include "transactionalDatabaseClient.h"
// IWYU pragma: no_include "resourcePool.impl.h"

namespace IceTray {
	template class TransactionalDatabaseClient<DB::ConnectionPtr>;
	template class TransactionalDatabaseClient<AdHoc::ResourceHandle<DB::Connection>>;
}
