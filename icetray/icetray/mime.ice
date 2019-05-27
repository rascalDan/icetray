#ifndef ICETRAY_MIME
#define ICETRAY_MIME

[["ice-prefix"]]
[["cpp:include:stream_support.h"]]
module IceTray {
	module Mime {
		local dictionary<string, string> Headers;

		local interface Writable {
			["cpp:const"] void write(["cpp:type:StreamPtr"] string buffer, int depth);
		};

		local class BasicPart implements Writable {
			Headers headers;
		};

		local sequence<BasicPart> Parts;

		local class BasicSinglePart extends BasicPart {
			string mimetype;
		};

		local class BasicMultiPart extends BasicPart {
			string subtype;
			Parts parts;
		};
	};
};

#endif

