#include "abstractDatabaseClient.h"
#include "Ice/Config.h"
#include "IceUtil/Optional.h"

namespace IceTray {
	AbstractDatabaseClient::AbstractDatabaseClient(const DB::ConnectionPoolPtr & d) :
		db(d)
	{
	}
}

namespace DB {
	template<>
	void Command::bindParam(unsigned int offset, const Ice::ByteSeq & p)
	{
		this->bindParamBLOB(offset, p);
	}
}

