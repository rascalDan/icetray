#include "abstractDatabaseClient.h"
#include "Ice/Config.h"
#include "IceUtil/Optional.h"
#include <Ice/BuiltinSequences.h>

namespace IceTray {
	AbstractDatabaseClient::AbstractDatabaseClient(DatabasePoolPtr d) :
		db(d)
	{
	}

#define PARAMBINDER(T, bindFunc) \
	template<> \
	void \
	AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const T & p) \
	{ \
		cmd->bindFunc(o, p); \
	}
	PARAMBINDER(std::string, bindParamS);
	PARAMBINDER(Ice::Byte, bindParamI);
	PARAMBINDER(Ice::Short, bindParamI);
	PARAMBINDER(Ice::Int, bindParamI);
	PARAMBINDER(Ice::Long, bindParamI);
	PARAMBINDER(unsigned long, bindParamI);
	PARAMBINDER(Ice::Float, bindParamF);
	PARAMBINDER(Ice::Double, bindParamF);
	PARAMBINDER(bool, bindParamB);
	PARAMBINDER(Ice::ByteSeq, bindParamBLOB);
	template<>
	void
	AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const std::nullptr_t &) \
	{
		cmd->bindNull(o);
	}

	void
	AbstractDatabaseClient::bind(int, DB::Command *)
	{
	}
}

