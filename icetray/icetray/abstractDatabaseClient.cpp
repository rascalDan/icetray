#include "abstractDatabaseClient.h"
#include <connectionPool.h>
#include <utility>

namespace IceTray {
	AbstractDatabaseClient::AbstractDatabaseClient(DB::ConnectionPoolPtr d) : db(std::move(d)) { }
}
