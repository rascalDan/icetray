#include "staticSqlSource.h"
#include <command.h>

namespace IceTray {
	StaticSqlSource::StaticSqlSource(const std::string & s) :
		sql(s),
		opts(new DB::CommandOptions(std::hash<std::string>()(sql)))
	{
	}

	StaticSqlSource::StaticSqlSource(const std::string & s, const std::string & optsName, const DB::CommandOptionsMap & map) :
		sql(s),
		opts(DB::CommandOptionsFactory::createNew(optsName, std::hash<std::string>()(sql), map))
	{

	}

	StaticSqlSource::~StaticSqlSource()
	{
		delete opts;
	}

	const std::string &
	StaticSqlSource::getSql() const
	{
		return sql;
	}

	const DB::CommandOptions *
	StaticSqlSource::getCommandOptions() const
	{
		return opts;
	}
}

