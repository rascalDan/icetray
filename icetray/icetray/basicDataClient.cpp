#include "basicDataClient.h"
#include <command.h>

namespace DB {
	template<>
	void
	Command::bindParam(unsigned int offset, const Ice::ByteSeq & p)
	{
		this->bindParamBLOB(offset, p);
	}
}
