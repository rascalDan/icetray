#include "abstractDatabaseClient.h"

namespace IceTray {
	AbstractDatabaseClient::AbstractDatabaseClient(DatabasePoolPtr d) :
		db(d)
	{
	}

	template<>
	void
	AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const std::string & p)
	{
		cmd->bindParamS(o, p);
	}

	template<>
	void
	AbstractDatabaseClient::bind1(int o, DB::Command * cmd, const int & p)
	{
		cmd->bindParamI(o, p);
	}

	void
	AbstractDatabaseClient::bind(int, DB::Command *)
	{
	}
}

