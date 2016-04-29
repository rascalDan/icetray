#include "sqlSource.h"

namespace IceTray {
	std::size_t
	SqlSource::getSqlHash() const
	{
		return std::hash<std::string>()(getSql());
	}
}

