#include "staticSqlSource.h"

namespace IceTray {
	StaticSqlSource::StaticSqlSource(const std::string & s) :
		sql(s),
		hash(std::hash<std::string>()(sql))
	{
	}

	const std::string &
	StaticSqlSource::getSql() const
	{
		return sql;
	}

	std::size_t
	StaticSqlSource::getSqlHash() const
	{
		return hash;
	}
}

