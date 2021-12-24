#include <Encoding/Base16.hpp>
#include <cassert>

using namespace std::string_literals;
using namespace std::string_view_literals;

void
testDecode(std::string_view const& encoded, std::string_view const& expected)
{
	Stream::BufferInput bufferInput{encoded.data(), encoded.size()};
	Encoding::Base16Decode base64Decode;
	bufferInput > base64Decode;

	std::string decoded;
	decoded.resize(expected.length());
	base64Decode.read(decoded.data(), expected.length());
	assert(decoded == expected);
}

void
testDecode(std::pair<std::string_view, std::string_view> const& encoded, std::string_view const& expected)
{
	Stream::BufferInput bufferInputFirst{encoded.first.data(), encoded.first.size()};
	Stream::BufferInput bufferInputLast{encoded.second.data(), encoded.second.size()};
	Encoding::Base16Decode base64Decode;
	bufferInputLast > bufferInputFirst > base64Decode;

	std::string decoded;
	decoded.resize(expected.length());
	base64Decode.read(decoded.data(), expected.length());
	assert(decoded == expected);
}


int main()
{
	try { // not enough data
		testDecode(""sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data
		testDecode("#"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}

	try { // not enough data
		testDecode("3"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data
		testDecode("3#"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}
	try { // not enough valid data
		testDecode(std::make_pair("3"sv, "#"sv), "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}


	try { // not enough data
		testDecode("31"sv, "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}


	testDecode("31"sv, "1"sv);
	testDecode("3132"sv, "12"sv);
	testDecode("313233"sv, "123"sv);

	return 0;
}
