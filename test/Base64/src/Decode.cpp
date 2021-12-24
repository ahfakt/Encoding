#include <Encoding/Base64.hpp>
#include <cassert>

using namespace std::string_literals;
using namespace std::string_view_literals;

void
testDecode(Encoding::Base64Decode::Mode const decodeMode, std::string_view const& encoded, std::string_view const& expected)
{
	Stream::BufferInput bufferInput{encoded.data(), encoded.size()};
	Encoding::Base64Decode base64Decode(decodeMode);
	bufferInput > base64Decode;

	std::string decoded;
	decoded.resize(expected.length());
	base64Decode.read(decoded.data(), expected.length());
	assert(decoded == expected);
}

void
testDecode(Encoding::Base64Decode::Mode const decodeMode, std::pair<std::string_view, std::string_view> const& encoded, std::string_view const& expected)
{
	Stream::BufferInput bufferInputFirst{encoded.first.data(), encoded.first.size()};
	Stream::BufferInput bufferInputLast{encoded.second.data(), encoded.second.size()};
	Encoding::Base64Decode base64Decode(decodeMode);
	bufferInputLast > bufferInputFirst > base64Decode;

	std::string decoded;
	decoded.resize(expected.length());
	base64Decode.read(decoded.data(), expected.length());
	assert(decoded == expected);
}


int main()
{
	try { // not enough data
		testDecode(Encoding::Base64Decode::Mode::UTF7, ""sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data
		testDecode(Encoding::Base64Decode::Mode::UTF7, "#"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}

	try { // not enough data
		testDecode(Encoding::Base64Decode::Mode::UTF7, "M"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data
		testDecode(Encoding::Base64Decode::Mode::UTF7, "M#"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}
	try { // not enough valid data
		testDecode(Encoding::Base64Decode::Mode::UTF7, std::make_pair("M"sv, "#"sv), "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}


	try { // not enough data
		testDecode(Encoding::Base64Decode::Mode::STD, "MQ="sv, "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data, invalid padding
		testDecode(Encoding::Base64Decode::Mode::STD, "MQ=#"sv, "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}
	try { // not enough valid data, invalid padding
		testDecode(Encoding::Base64Decode::Mode::STD, std::make_pair("MQ="sv, "A"sv), "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}


	testDecode(Encoding::Base64Decode::Mode::UTF7, "MQ"sv, "1"sv);
	testDecode(Encoding::Base64Decode::Mode::UTF7, "MTI"sv, "12"sv);
	testDecode(Encoding::Base64Decode::Mode::UTF7, "MTIz"sv, "123"sv);
	testDecode(Encoding::Base64Decode::Mode::UTF7, "MTIzNA"sv, "1234"sv);

	testDecode(Encoding::Base64Decode::Mode::STD, "MQ=="sv, "1"sv);
	testDecode(Encoding::Base64Decode::Mode::STD, "MTI="sv, "12"sv);
	testDecode(Encoding::Base64Decode::Mode::STD, "MTIz"sv, "123"sv);
	testDecode(Encoding::Base64Decode::Mode::STD, "MTIzNA=="sv, "1234"sv);

	// buffer overrun
	testDecode(Encoding::Base64Decode::Mode::STD, "MTI="sv, "1"sv);
	testDecode(Encoding::Base64Decode::Mode::STD, std::make_pair("MT"sv, "Iz"sv), "12"sv);

	return 0;
}
