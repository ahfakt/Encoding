#include <Encoding/Base32.hpp>
#include <cassert>

using namespace std::string_literals;
using namespace std::string_view_literals;

void
testEncode(Encoding::Base32Encode::Mode const encodeMode, std::string_view const& decoded, std::string_view const& expected)
{
	std::string encoded;
	encoded.resize(expected.length());
	Stream::BufferOutput bufferOutput{encoded.data(), encoded.size()};
	Stream::StdOut < bufferOutput;
	{
		Encoding::Base32Encode base32Encode(encodeMode);
		bufferOutput < base32Encode;
		base32Encode.write(decoded.data(), decoded.length());
	}
	bufferOutput << '\n';
	assert(encoded == expected);
}

int main ()
{
	testEncode(Encoding::Base32Encode::Mode::STDN, "1"sv, "GE"sv);
	testEncode(Encoding::Base32Encode::Mode::STDN, "12"sv, "GEZA"sv);
	testEncode(Encoding::Base32Encode::Mode::STDN, "123"sv, "GEZDG"sv);
	testEncode(Encoding::Base32Encode::Mode::STDN, "1234"sv, "GEZDGNA"sv);
	testEncode(Encoding::Base32Encode::Mode::STDN, "12345"sv, "GEZDGNBV"sv);
	testEncode(Encoding::Base32Encode::Mode::STDN, "123456"sv, "GEZDGNBVGY"sv);

	testEncode(Encoding::Base32Encode::Mode::STD, "1"sv, "GE======"sv);
	testEncode(Encoding::Base32Encode::Mode::STD, "12"sv, "GEZA===="sv);
	testEncode(Encoding::Base32Encode::Mode::STD, "123"sv, "GEZDG==="sv);
	testEncode(Encoding::Base32Encode::Mode::STD, "1234"sv, "GEZDGNA="sv);
	testEncode(Encoding::Base32Encode::Mode::STD, "12345"sv, "GEZDGNBV"sv);
	testEncode(Encoding::Base32Encode::Mode::STD, "123456"sv, "GEZDGNBVGY======"sv);

	return 0;
}
