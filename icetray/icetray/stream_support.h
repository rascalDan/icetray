#ifndef ICETRAY_STREAM_SUPPORT_H
#define ICETRAY_STREAM_SUPPORT_H

#include <cstdio>
#include <memory>

namespace IceTray {
	class StreamPtr {
	public:
		operator FILE *() const
		{
			return f;
		}
		FILE * const f;
	};
}

#endif
