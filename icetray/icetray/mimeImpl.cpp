#include "mimeImpl.h"
#include <string_view>

namespace IceTray::Mime {
	static const char * const DIVIDER = "//divider//";
	static const std::string_view mime_base64 =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	void
	PartHelper::writeHeaders(const Headers & headers, const StreamPtr & ms)
	{
		for (const auto & h : headers) {
			if (h.second.empty()) {
				continue;
			}
			fprintf(ms, "%s: %s\r\n", h.first.c_str(), h.second.c_str());
		}
	}

	TextPart::TextPart(const Headers & h, const std::string & m, std::string p) :
		BasicSinglePart(h, m),
		payload(std::move(p))
	{
	}

	void
	TextPart::write(const StreamPtr & ms, Ice::Int) const
	{
		writeHeaders(headers, ms);
		fputs("Content-Transfer-Encoding: quoted-printable\r\n", ms);
		fprintf(ms, "Content-Type: %s; charset=\"utf-8\"\r\n\r\n",
				mimetype.c_str());
		quotedPrintable(payload, ms);
		fputs("\r\n", ms);
	}

	void
	TextPart::quotedPrintable(const std::string_view & input, FILE * ms, const size_t maxWidth)
	{
		size_t line = 0;
		auto wrap = [&]() {
			fputs("=\r\n", ms);
			line = 0;
		};
		auto wrapIfNeeded = [&](size_t need) {
			if (line + need > maxWidth) {
				wrap();
			}
		};
		for (const auto & ch : input) {
			const auto & nextCh = *(&ch + 1);
			if (ch == '\r') {
			}
			else if (ch == '\n') {
				fputs("\r\n", ms);
				line = 0;
			}
			else if (ch == ' ' || ch == '\t') {
				if (nextCh == '\r' || nextCh == '\n') {
					wrapIfNeeded(3);
					fprintf(ms, "=%02X", (uint8_t)ch);
					line += 3;
				}
				else {
					wrapIfNeeded(1);
					fputc(ch, ms);
					line += 1;
				}
			}
			else if ((ch >= 33 && ch < 61) || (ch > 61 && ch <= 126)) {
				wrapIfNeeded(1);
				fputc(ch, ms);
				line += 1;
			}
			else {
				wrapIfNeeded(3);
				fprintf(ms, "=%02X", (uint8_t)ch);
				line += 3;
			}
		}
	}

	BinaryViewPart::BinaryViewPart(const Headers & h, const std::string & m, const byte_range & p) :
		BasicSinglePart(h, m),
		payload(p)
	{
	}

	void
	BinaryViewPart::write(const StreamPtr & ms, Ice::Int) const
	{
		writeHeaders(headers, ms);
		fputs("Content-Transfer-Encoding: base64\r\n", ms);
		fprintf(ms, "Content-Type: %s\r\n\r\n",
				mimetype.c_str());
		base64(payload, ms);
		fputs("\r\n", ms);
	}

	void
	BinaryViewPart::base64(const std::basic_string_view<uint8_t> & input, FILE * ms,
					const size_t maxWidth)
	{
		auto mime_encode_base64_block = [](auto & dest, const auto & src) {
			if (src.length() >= 1) {
				dest[0] = mime_base64[(src[0] & 0xFCu) >> 2u];
				if (src.length() >= 2) {
					dest[1] = mime_base64[((src[0] & 0x03u) << 4u) | ((src[1] & 0xF0u) >> 4u)];
					if (src.length() >= 3) {
						dest[2] = mime_base64[((src[1] & 0x0Fu) << 2u) | ((src[2] & 0xC0u) >> 6u)];
						dest[3] = mime_base64[((src[2] & 0x3Fu))];
					}
					else {
						dest[2] = mime_base64[((src[1] & 0x0Fu) << 2u)];
					}
				}
				else {
					dest[1] = mime_base64[(src[0] & 0x03u) << 4u];
				}
			}
		};
		
		size_t l = 0;
		for (size_t i = 0; i < input.length(); i += 3) {
			if (maxWidth > 0 && l + 4 > maxWidth) {
				fputs("\r\n", ms);
				l = 0;
			}

			std::array<char, 4> bytes { '=', '=', '=', '=' };
			mime_encode_base64_block(bytes, input.substr(i, 3));
			fwrite(bytes.data(), bytes.size(),1, ms);
			l += 4;
		}
		fputs("\r\n", ms);
	}

	BinaryCopyPart::BinaryCopyPart(const Headers & h, const std::string & m, bytes v) :
		BinaryViewPart(h, m, { v.data(), v.size() }),
		payload(std::move(v))
	{
	}


	MultiPart::MultiPart(const Headers & h, const std::string & st, const Parts & p) :
		BasicMultiPart(h, st, p)
	{
	}

	void
	MultiPart::write(const StreamPtr & ms, Ice::Int depth) const
	{
		writeHeaders(headers, ms);
		fprintf(ms, "Content-Type: multipart/%s; boundary=\"%s%d\"\r\n\r\n",
				subtype.c_str(), DIVIDER, depth);
		for (const auto & p : parts) {
			fprintf(ms, "--%s%d\r\n", DIVIDER, depth);
			p->write(ms, depth + 1);
		}
		fprintf(ms, "--%s%d--\r\n", DIVIDER, depth);
	}

}

