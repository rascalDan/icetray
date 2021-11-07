#ifndef ICETRAY_MIME_IMPL_H
#define ICETRAY_MIME_IMPL_H

#include "mime.h"
#include <Ice/Config.h>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>
#include <visibility.h>

namespace IceTray {
	class StreamPtr;
}
namespace IceTray::Mime {
	class DLL_PUBLIC PartHelper {
	protected:
		static void writeHeaders(const Headers & headers, const StreamPtr & ms);
	};

	class DLL_PUBLIC TextPart : public BasicSinglePart, PartHelper {
	public:
		TextPart(const Headers &, const std::string &, std::string);

		void write(const StreamPtr & ms, Ice::Int depth) const override;

		static void quotedPrintable(const std::string_view & input, FILE * ms, const size_t maxWidth = 74);

		const std::string payload;
	};

	class DLL_PUBLIC BinaryViewPart : public BasicSinglePart, PartHelper {
	public:
		using byte = uint8_t;
		using byte_range = std::basic_string_view<byte>;

		BinaryViewPart(const Headers &, const std::string &, const byte_range &);

		void write(const StreamPtr & ms, Ice::Int depth) const override;

		static void base64(const byte_range & input, FILE * ms, const size_t maxWidth = 76);

		const byte_range payload;
	};

	class DLL_PUBLIC BinaryCopyPart : public BinaryViewPart {
	public:
		using bytes = std::vector<byte>;

		BinaryCopyPart(const Headers &, const std::string &, bytes);

		const bytes payload_data;
	};

	class DLL_PUBLIC MultiPart : public BasicMultiPart, PartHelper {
	public:
		MultiPart(const Headers &, const std::string &, const Parts &);

		void write(const StreamPtr & ms, Ice::Int depth) const override;
	};
}

#endif
