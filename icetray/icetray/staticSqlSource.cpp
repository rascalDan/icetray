#include "staticSqlSource.h"
#include <command.h>
#include <command_fwd.h>
#include <utility>

namespace IceTray {
	StaticSqlSource::StaticSqlSource(std::string s) :
		sql(std::move(s)), opts(new DB::CommandOptions(std::hash<std::string>()(sql)))
	{
	}

	StaticSqlSource::StaticSqlSource(std::string s, const std::string & optsName, const DB::CommandOptionsMap & map) :
		sql(std::move(s)), opts(DB::CommandOptionsFactory::createNew(optsName, std::hash<std::string>()(sql), map))
	{
	}

	const std::string &
	StaticSqlSource::getSql() const
	{
		return sql;
	}

	DB::CommandOptionsCPtr
	StaticSqlSource::getCommandOptions() const
	{
		return opts;
	}
}
