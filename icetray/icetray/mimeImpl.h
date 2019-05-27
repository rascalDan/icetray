#ifndef ICETRAY_MIME_IMPL_H
#define ICETRAY_MIME_IMPL_H

#include <mime.h>
#include <visibility.h>

namespace IceTray::Mime {
	class DLL_PUBLIC PartHelper {
		protected:
			static void writeHeaders(const Headers & headers, const StreamPtr & ms);
	};

	class DLL_PUBLIC TextPart : public BasicSinglePart, PartHelper {
		public:
			TextPart(const Headers &, const std::string &, const std::string &);

			void write(const StreamPtr & ms, Ice::Int depth) const override;

			static void quotedPrintable(const std::string_view & input, FILE * ms,
					const size_t maxWidth = 74);

			const std::string payload;
	};

	class DLL_PUBLIC BinaryViewPart : public BasicSinglePart, PartHelper {
		public:
			BinaryViewPart(const Headers &, const std::string &, const std::basic_string_view<uint8_t> &);

			void write(const StreamPtr & ms, Ice::Int depth) const override;

			static void base64(const std::basic_string_view<uint8_t> & input, FILE * ms,
					const size_t maxWidth = 76);

			std::basic_string_view<uint8_t> payload;
	};

	class DLL_PUBLIC BinaryCopyPart : public BinaryViewPart {
		public:
			BinaryCopyPart(const Headers &, const std::string &, std::vector<uint8_t>);

			std::vector<uint8_t> payload;
	};

	class DLL_PUBLIC MultiPart : public BasicMultiPart, PartHelper {
		public:
			MultiPart(const Headers &, const std::string &, const Parts &);

			void write(const StreamPtr & ms, Ice::Int depth) const override;
	};
}

#endif

