#include "abstractDatabaseClient.h"
#include <command.h>
#include <connectionPool.h>
#include <utility>

namespace IceTray {
	AbstractDatabaseClient::AbstractDatabaseClient(DB::ConnectionPoolPtr d) : db(std::move(d)) { }
}

namespace DB {
	template<>
	void
	Command::bindParam(unsigned int offset, const Ice::ByteSeq & p)
	{
		this->bindParamBLOB(offset, p);
	}
}
