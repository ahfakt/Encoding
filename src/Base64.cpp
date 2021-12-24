#include "Encoding/Base64.hpp"


namespace Encoding {

Base64Decode::Base64Decode(Mode decodeMode)
		: mDecodeMode{static_cast<std::uint8_t>(decodeMode)}
		, mPrevConsumed{0}
{}

Base64Decode::Base64Decode(Base64Decode&& other) noexcept
{ swap(*this, other); }

void
swap(Base64Decode& a, Base64Decode& b) noexcept
{
	swap(static_cast<Stream::BufferReader&>(a), static_cast<Stream::BufferReader&>(b));
	std::swap(a.mDecodeMode, b.mDecodeMode);
	std::swap(a.mPrevConsumed, b.mPrevConsumed);
	std::swap(a.mPartialDecoded, b.mPartialDecoded);
}

Base64Decode&
Base64Decode::operator=(Base64Decode&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::uint8_t const
DecodeTable[][256]{
{ // STD, UTF7, PEM, MIME
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
	255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
{ // URL
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62, 255, 255,
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
	255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255,  63,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
{ // IMAP
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  62,  63, 255, 255, 255,
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 255, 255, 255, 255,
	255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}
};

std::size_t
Base64Decode::readBytes(std::byte* const dest, std::size_t size)
{
	// provide some data to decode
	auto const prevProduced{::floorz(mPrevConsumed * Base64::DecodedLength, Base64::EncodedLength)}; // to dest
	auto const requested{::ceilz((prevProduced + size) * Base64::EncodedLength, Base64::DecodedLength) - mPrevConsumed};

	std::size_t available;
	std::uint8_t const* src;
	if (mDecodeMode & 4) { // padding allowed
		available = getSource().provideSome(::ceilz((prevProduced + size), Base64::DecodedLength) * Base64::EncodedLength - mPrevConsumed); // ask for block aligned data
		src = reinterpret_cast<std::uint8_t const*>(getSource().begin());
		
		if (mPrevConsumed > 0 && src[0] == '=') { // skip valid padding
			std::size_t padding{1};
			if (auto const required{Base64::EncodedLength - mPrevConsumed}; padding < required) {
				if (available < required) {
					available = getSource().provideSomeMore(1/*requiredPadding - available*/);
					src = reinterpret_cast<std::uint8_t const*>(getSource().begin()); // the buffer might have changed
				}
				if (src[padding] != '=') [[unlikely]]
					throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};
				++padding;
			}

			mPrevConsumed = 0;
			getSource().consumed(padding);
			return 0; // retry
		}
	} else {
		available = getSource().provideSome(requested);
		src = reinterpret_cast<std::uint8_t const*>(getSource().begin());
	}


	// find valid input size
	auto const* const decodeTable{reinterpret_cast<std::byte const*>(DecodeTable[mDecodeMode & 3])}; // alphabet
	if (decodeTable[src[0]] == std::byte{255}) [[unlikely]]
		throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};

	std::size_t valid{1};
	if (auto const required{::ceilz((prevProduced + 1) * Base64::EncodedLength, Base64::DecodedLength) - mPrevConsumed}; valid < required) {
		if (available < required) {
			available = getSource().provideSomeMore(1/*required - available*/);
			src = reinterpret_cast<std::uint8_t const*>(getSource().begin()); // the buffer might have changed
		}
		if (decodeTable[src[valid]] == std::byte{255}) [[unlikely]]
			throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};
		++valid;
	}

	// Due to padding, more data than required for decoding may have been requested to ensure block alignment.
	// If the data after the required length is not padding but valid data, it may cause decoding more than requested.
	// To prevent this, check up to the shorter value instead of the entire available data.
	for (auto const min{std::min(available, requested)}; valid < min && decodeTable[src[valid]] != std::byte{255}; ++valid);

	size  = ::floorz((mPrevConsumed + valid) * Base64::DecodedLength, Base64::EncodedLength) - prevProduced;
	valid = ::ceilz((prevProduced + size) * Base64::EncodedLength, Base64::DecodedLength) - mPrevConsumed;


	// decode
	std::size_t inl{0};
	std::size_t outl{0};

	switch (mPrevConsumed) { // decode incomplete block first
		case 2: {
			dest[outl++] = decodeTable[mPartialDecoded] << 4 | decodeTable[src[inl]] >> 2;
			++mPrevConsumed;
			mPartialDecoded = src[inl];
			if (++inl == valid)
				goto afterDecode;
		}
		case 3: {
			dest[outl++] = decodeTable[mPartialDecoded] << 6 | decodeTable[src[inl++]];
			mPrevConsumed = 0;
		}
	}

	while (valid - inl >= Base64::EncodedLength) { // decode full blocks
		dest[outl    ] = decodeTable[src[inl    ]] << 2 | decodeTable[src[inl + 1]] >> 4;
		dest[outl + 1] = decodeTable[src[inl + 1]] << 4 | decodeTable[src[inl + 2]] >> 2;
		dest[outl + 2] = decodeTable[src[inl + 2]] << 6 | decodeTable[src[inl + 3]]     ;
		outl += Base64::DecodedLength;
		inl  += Base64::EncodedLength;
	}

	if (valid - inl > 1) { // decode remaining partial block
		dest[outl++] = decodeTable[src[inl]] << 2 | decodeTable[src[inl + 1]] >> 4;
		if (valid - inl > 2) {
			dest[outl++] = decodeTable[src[inl + 1]] << 4 | decodeTable[src[inl + 2]] >> 2;
			mPartialDecoded = src[(inl += (mPrevConsumed = 3)) - 1];
		} else
			mPartialDecoded = src[(inl += (mPrevConsumed = 2)) - 1];
	}

	afterDecode:
	if (mDecodeMode & 4 && mPrevConsumed > 0) { // padding allowed
		auto const required{Base64::EncodedLength - mPrevConsumed};
		if (valid + required <= available) { // there is enough data to check valid padding
			while (src[valid] == '=') {
				if (++valid - inl == required) {
					mPrevConsumed = 0;
					inl += required;
					break;
				}
			}
		}
	}

	getSource().consumed(inl);
	return outl;
}


Base64Encode::Base64Encode(Mode encodeMode)
		: mEncodeMode{static_cast<std::uint8_t>(encodeMode)}
		, mPrevConsumed{0}
{}

Base64Encode::Base64Encode(Base64Encode&& other) noexcept
{ swap(*this, other); }

void
swap(Base64Encode& a, Base64Encode& b) noexcept
{
	swap(static_cast<Stream::BufferWriter&>(a), static_cast<Stream::BufferWriter&>(b));
	std::swap(a.mEncodeMode, b.mEncodeMode);
	std::swap(a.mPrevConsumed, b.mPrevConsumed);
	std::swap(a.mPartialEncoded, b.mPartialEncoded);
}

Base64Encode&
Base64Encode::operator=(Base64Encode&& other) noexcept
{
	swap(*this, other);
	return *this;
}

Base64Encode::~Base64Encode()
{
	try {
		flush();
	} catch (Output::Exception const& exc) {
		// Nothing can be done
		LOG_ERR(exc.what());
	}
}

/**
 * @see		<a href="https://tools.ietf.org/html/rfc4648#section-3.4">Choosing the Alphabet</a>
 */
std::uint8_t const
EncodeTable[][64]{
{ // STD, UTF7, PEM, MIME
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'},
{ // URL
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'},
{ // IMAP
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', ','}
};

std::size_t
Base64Encode::writeBytes(std::byte const* s, std::size_t size)
{
	// allocate some space to encode
	auto const prevProduced{::floorz(mPrevConsumed * Base64::EncodedLength, Base64::DecodedLength)}; // from src
	auto const requested{::ceilz((mPrevConsumed + size) * Base64::EncodedLength, Base64::DecodedLength) - prevProduced};

	std::size_t available;
	std::size_t required;
	if (mEncodeMode & 4) { // padding allowed
		available = getSink().allocSome(::ceilz((mPrevConsumed + size), Base64::DecodedLength) * Base64::EncodedLength - prevProduced); // ask for block aligned space
		required = ::ceilz((mPrevConsumed + 1), Base64::DecodedLength) * Base64::EncodedLength - prevProduced;
	} else {
		available = getSink().allocSome(requested); // ask for requested space
		required = ::ceilz((mPrevConsumed + 1) * Base64::EncodedLength, Base64::DecodedLength) - prevProduced;
	}
	if (available < required)
		available = getSink().alloc(required);
	auto* dest{getSink().begin()};

	// find valid output size
	size = ::floorz((prevProduced + std::min(available, requested)) * Base64::DecodedLength, Base64::EncodedLength) - mPrevConsumed;

	// encode
	std::size_t inl{0};
	std::size_t outl{0};
	auto const* const encodeTable{reinterpret_cast<std::byte const*>(EncodeTable[mEncodeMode & 3])}; // alphabet
	auto const* src{reinterpret_cast<std::uint8_t const*>(s)};

	switch (mPrevConsumed) { // encode incomplete block first
		case 1: {
			dest[outl++] = encodeTable[(mPartialEncoded & 0x03) << 4 | src[inl] >> 4];
			++mPrevConsumed;
			mPartialEncoded = src[inl];
			if (++inl == size)
				goto afterEncode;
		}
		case 2: {
			dest[outl++] = encodeTable[(mPartialEncoded & 0x0F) << 2 | src[inl] >> 6];
			dest[outl++] = encodeTable[(src[inl] & 0x3F)];
			mPrevConsumed = 0;
		}
	}

	while (size - inl >= Base64::DecodedLength) { // encode full blocks
		dest[outl    ] = encodeTable[ src[inl    ]         >> 2                    ];
		dest[outl + 1] = encodeTable[(src[inl    ] & 0x03) << 4 | src[inl + 1] >> 4];
		dest[outl + 2] = encodeTable[(src[inl + 1] & 0x0F) << 2 | src[inl + 2] >> 6];
		dest[outl + 3] = encodeTable[ src[inl + 2] & 0x3F                          ];
		outl += Base64::EncodedLength;
		inl  += Base64::DecodedLength;
	}

	if (size - inl > 0) { // encode remaining partial block
		dest[outl++] = encodeTable[src[inl] >> 2];
		if (size - inl > 1) {
			dest[outl++] = encodeTable[(src[inl] & 0x03) << 4 | src[inl + 1] >> 4];
			mPartialEncoded = src[(inl += (mPrevConsumed = 2)) - 1];
		} else
			mPartialEncoded = src[(inl += (mPrevConsumed = 1)) - 1];
	}

	afterEncode:
	getSink().produced(outl);
	return inl;
}

void
Base64Encode::flush()
{
	if (mPrevConsumed > 0) {
		// allocate required space to encode
		std::size_t required;
		if (mEncodeMode & 4) // padding allowed
			getSink().alloc(required = Base64::EncodedLength - ::floorz(mPrevConsumed * Base64::EncodedLength, Base64::DecodedLength)); // ask for block aligned space
		else
			getSink().allocSome(required = 1); // ask for required space
		auto* dest{getSink().begin()};

		// encode remaining bits of last byte
		auto const* const encodeTable{reinterpret_cast<std::byte const*>(EncodeTable[mEncodeMode & 3])}; // alphabet
		switch (mPrevConsumed) {
			case 1: {
				dest[0] = encodeTable[(mPartialEncoded & 0x03) << 4];
			} break;
			case 2: {
				dest[0] = encodeTable[(mPartialEncoded & 0x0F) << 2];
			}
		}
		mPrevConsumed = 0;

		// write padding if required
		for (std::size_t i{1}; i < required; ++i) // padding allowed
			dest[i] = std::byte{'='};
		getSink().produced(required);
	}
}


Base64::Base64(Base64Decode::Mode decodeMode, Base64Encode::Mode encodeMode)
		: Base64Decode{decodeMode}
		, Base64Encode{encodeMode}
{}

void
swap(Base64& a, Base64& b) noexcept
{
	swap(static_cast<Base64Decode&>(a), static_cast<Base64Decode&>(b));
	swap(static_cast<Base64Encode&>(a), static_cast<Base64Encode&>(b));
}

}//namespace Encoding
