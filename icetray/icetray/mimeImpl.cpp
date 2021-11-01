#include "mimeImpl.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <string_view>

namespace IceTray::Mime {
	static const char * const DIVIDER = "//divider//";

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
		BasicSinglePart(h, m), payload(std::move(p))
	{
	}

	void
	TextPart::write(const StreamPtr & ms, Ice::Int) const
	{
		writeHeaders(headers, ms);
		fputs("Content-Transfer-Encoding: quoted-printable\r\n", ms);
		fprintf(ms, "Content-Type: %s; charset=\"utf-8\"\r\n\r\n", mimetype.c_str());
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
			if (ch == '\r') { }
			else if (ch == '\n') {
				fputs("\r\n", ms);
				line = 0;
			}
			else if (ch == ' ' || ch == '\t') {
				if (nextCh == '\r' || nextCh == '\n') {
					wrapIfNeeded(3);
					fprintf(ms, "=%02X", static_cast<uint8_t>(ch));
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
				fprintf(ms, "=%02X", static_cast<uint8_t>(ch));
				line += 3;
			}
		}
	}

	BinaryViewPart::BinaryViewPart(const Headers & h, const std::string & m, const byte_range & p) :
		BasicSinglePart(h, m), payload(p)
	{
	}

	void
	BinaryViewPart::write(const StreamPtr & ms, Ice::Int) const
	{
		writeHeaders(headers, ms);
		fputs("Content-Transfer-Encoding: base64\r\n", ms);
		fprintf(ms, "Content-Type: %s\r\n\r\n", mimetype.c_str());
		base64(payload, ms);
		fputs("\r\n", ms);
	}

	void
	BinaryViewPart::base64(const std::basic_string_view<uint8_t> & input, FILE * ms, const size_t maxWidth)
	{
		if (input.empty()) {
			fputs("\r\n", ms);
			return;
		}
		auto blockSize = (maxWidth * 3) / 4;
		auto b64 = std::unique_ptr<BIO, decltype(&BIO_free_all)> {BIO_new(BIO_f_base64()), &BIO_free_all};
		BIO_push(b64.get(), BIO_new_fp(ms, BIO_NOCLOSE));
		BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
		auto left = input.length();
		for (auto start = input.data(); start < input.end(); start += blockSize, left -= blockSize) {
			BIO_write(b64.get(), start, static_cast<int>(std::min(blockSize, left)));
			BIO_flush(b64.get());
			fputs("\r\n", ms);
		}
	}

	BinaryCopyPart::BinaryCopyPart(const Headers & h, const std::string & m, bytes v) :
		BinaryViewPart(h, m, {v.data(), v.size()}), payload_data(std::move(v))
	{
	}

	MultiPart::MultiPart(const Headers & h, const std::string & st, const Parts & p) : BasicMultiPart(h, st, p) { }

	void
	MultiPart::write(const StreamPtr & ms, Ice::Int depth) const
	{
		writeHeaders(headers, ms);
		fprintf(ms, "Content-Type: multipart/%s; boundary=\"%s%d\"\r\n\r\n", subtype.c_str(), DIVIDER, depth);
		for (const auto & p : parts) {
			fprintf(ms, "--%s%d\r\n", DIVIDER, depth);
			p->write(ms, depth + 1);
		}
		fprintf(ms, "--%s%d--\r\n", DIVIDER, depth);
	}

}
