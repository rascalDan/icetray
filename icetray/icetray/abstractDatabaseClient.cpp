#include "abstractDatabaseClient.h"
#include "transactionalDatabaseClient.h"
#include <connection.h>
#include <connectionPool.h>
#include <utility>
// IWYU pragma: no_include <map>

namespace IceTray {
	AbstractDatabaseClient::AbstractDatabaseClient(DB::ConnectionPoolPtr d) : db(std::move(d)) { }

	TransactionalDatabaseClient<AdHoc::ResourceHandle<DB::Connection>>
	AbstractDatabaseClient::transactional()
	{
		return TransactionalDatabaseClient {db->get()};
	}
}
