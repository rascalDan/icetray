#ifndef ICETRAY_SQLSOURCE_H
#define ICETRAY_SQLSOURCE_H

#include <functional>
#include <string>
#include <visibility.h>

namespace IceTray {
	class DLL_PUBLIC SqlSource {
		public:
			virtual ~SqlSource() = default;

			virtual const std::string & getSql() const = 0;
			virtual std::size_t getSqlHash() const;
	};
}

#endif

