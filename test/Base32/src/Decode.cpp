#include <Encoding/Base32.hpp>
#include <cassert>

using namespace std::string_literals;
using namespace std::string_view_literals;

void
testDecode(Encoding::Base32Decode::Mode const decodeMode, std::string_view const& encoded, std::string_view const& expected)
{
	Stream::BufferInput bufferInput{encoded.data(), encoded.size()};
	Encoding::Base32Decode Base32Decode(decodeMode);
	bufferInput > Base32Decode;

	std::string decoded;
	decoded.resize(expected.length());
	Base32Decode.read(decoded.data(), expected.length());
	assert(decoded == expected);
}

void
testDecode(Encoding::Base32Decode::Mode const decodeMode, std::pair<std::string_view, std::string_view> const& encoded, std::string_view const& expected)
{
	Stream::BufferInput bufferInputFirst{encoded.first.data(), encoded.first.size()};
	Stream::BufferInput bufferInputLast{encoded.second.data(), encoded.second.size()};
	Encoding::Base32Decode Base32Decode(decodeMode);
	bufferInputLast > bufferInputFirst > Base32Decode;

	std::string decoded;
	decoded.resize(expected.length());
	Base32Decode.read(decoded.data(), expected.length());
	assert(decoded == expected);
}


int main()
{
	try { // not enough data
		testDecode(Encoding::Base32Decode::Mode::STDN, ""sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data
		testDecode(Encoding::Base32Decode::Mode::STDN, "#"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}

	try { // not enough data
		testDecode(Encoding::Base32Decode::Mode::STDN, "G"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	try { // not enough valid data
		testDecode(Encoding::Base32Decode::Mode::STDN, "G#"sv, "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}
	try { // not enough valid data
		testDecode(Encoding::Base32Decode::Mode::STDN, std::make_pair("G"sv, "#"sv), "1"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}


	try { // not enough data
		testDecode(Encoding::Base32Decode::Mode::STD, "GE="sv, "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::no_message_available);
	}

	for (auto const& encoded : {"GE=#"sv, "GE==#"sv, "GE===#"sv, "GE====#"sv}) try { // not enough valid data, invalid padding
		testDecode(Encoding::Base32Decode::Mode::STD, encoded, "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}
	for (auto const& encoded : {std::make_pair("GE="sv, "A"sv), std::make_pair("GE=="sv, "A"sv), std::make_pair("GE==="sv, "A"sv), std::make_pair("GE===="sv, "A"sv)}) try { // not enough valid data, invalid padding
		testDecode(Encoding::Base32Decode::Mode::STD, encoded, "12"sv);
		assert(false);
	} catch (Stream::Input::Exception const& exc) {
		assert(exc.code() == std::errc::illegal_byte_sequence);
	}


	testDecode(Encoding::Base32Decode::Mode::STDN, "GE"sv, "1"sv);
	testDecode(Encoding::Base32Decode::Mode::STDN, "GEZA"sv, "12"sv);
	testDecode(Encoding::Base32Decode::Mode::STDN, "GEZDG"sv, "123"sv);
	testDecode(Encoding::Base32Decode::Mode::STDN, "GEZDGNA"sv, "1234"sv);
	testDecode(Encoding::Base32Decode::Mode::STDN, "GEZDGNBV"sv, "12345"sv);
	testDecode(Encoding::Base32Decode::Mode::STDN, "GEZDGNBVGY"sv, "123456"sv);

	testDecode(Encoding::Base32Decode::Mode::STD, "GE======"sv, "1"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZA===="sv, "12"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZDG==="sv, "123"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZDGNA="sv, "1234"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZDGNBV"sv, "12345"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZDGNBVGY======"sv, "123456"sv);

	// buffer overrun
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZA===="sv, "1"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, std::make_pair("GE"sv, "ZDG==="sv), "12"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, "GEZDGNA="sv, "123"sv);
	testDecode(Encoding::Base32Decode::Mode::STD, std::make_pair("GEZD"sv, "GNBV"sv), "1234"sv);

	return 0;
}
