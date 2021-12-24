#include <Encoding/Base16.hpp>
#include <cassert>

using namespace std::string_literals;
using namespace std::string_view_literals;

void
testEncode(std::string_view const& decoded, std::string_view const& expected)
{
	std::string encoded;
	encoded.resize(expected.length());
	Stream::BufferOutput bufferOutput{encoded.data(), encoded.size()};
	Stream::StdOut < bufferOutput;
	{
		Encoding::Base16Encode base16Encode;
		bufferOutput < base16Encode;
		base16Encode.write(decoded.data(), decoded.length());
	}
	bufferOutput << '\n';
	assert(encoded == expected);
}

int main ()
{
	testEncode("1"sv, "31"sv);
	testEncode("12"sv, "3132"sv);
	testEncode("123"sv, "313233"sv);
	return 0;
}
