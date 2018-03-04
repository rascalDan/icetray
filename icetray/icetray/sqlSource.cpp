#include "sqlSource.h"
#include <command.h>
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

