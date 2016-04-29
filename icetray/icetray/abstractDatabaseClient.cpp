#include "abstractDatabaseClient.h"
#include "Ice/Config.h"
#include "IceUtil/Optional.h"

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
	} \
	template<> \
	void \
	AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const IceUtil::Optional<T> & p) \
	{ \
		if (p) { \
			cmd->bindFunc(o, *p); \
		} \
		else { \
			cmd->bindNull(o); \
		} \
	}
	PARAMBINDER(std::string, bindParamS);
	PARAMBINDER(Ice::Byte, bindParamI);
	PARAMBINDER(Ice::Short, bindParamI);
	PARAMBINDER(Ice::Int, bindParamI);
	PARAMBINDER(Ice::Long, bindParamI);
	PARAMBINDER(Ice::Float, bindParamF);
	PARAMBINDER(Ice::Double, bindParamF);
	PARAMBINDER(bool, bindParamB);

	void
	AbstractDatabaseClient::bind(int, DB::Command *)
	{
	}
}

