#include <Encoding/Base64.hpp>
#include <cassert>

using namespace std::string_literals;
using namespace std::string_view_literals;

void
testEncode(Encoding::Base64Encode::Mode const encodeMode, std::string_view const& decoded, std::string_view const& expected)
{
	std::string encoded;
	encoded.resize(expected.length());
	Stream::BufferOutput bufferOutput{encoded.data(), encoded.size()};
	Stream::StdOut < bufferOutput;
	{
		Encoding::Base64Encode base64Encode(encodeMode);
		bufferOutput < base64Encode;
		base64Encode.write(decoded.data(), decoded.length());
	}
	bufferOutput << '\n';
	assert(encoded == expected);
}

int main ()
{
	testEncode(Encoding::Base64Encode::Mode::UTF7, "1"sv, "MQ"sv);
	testEncode(Encoding::Base64Encode::Mode::UTF7, "12"sv, "MTI"sv);
	testEncode(Encoding::Base64Encode::Mode::UTF7, "123"sv, "MTIz"sv);
	testEncode(Encoding::Base64Encode::Mode::UTF7, "1234"sv, "MTIzNA"sv);

	testEncode(Encoding::Base64Encode::Mode::STD, "1"sv, "MQ=="sv);
	testEncode(Encoding::Base64Encode::Mode::STD, "12"sv, "MTI="sv);
	testEncode(Encoding::Base64Encode::Mode::STD, "123"sv, "MTIz"sv);
	testEncode(Encoding::Base64Encode::Mode::STD, "1234"sv, "MTIzNA=="sv);

	return 0;
}
