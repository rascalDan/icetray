#include "sqlSource.h"
#include <command_fwd.h>
#include <connection.h>

namespace IceTray {
	DB::ModifyCommandPtr
	SqlSource::modify(DB::Connection * db) const
	{
		return db->modify(getSql(), getCommandOptions());
	}

	DB::SelectCommandPtr
	SqlSource::select(DB::Connection * db) const
	{
		return db->select(getSql(), getCommandOptions());
	}
}
