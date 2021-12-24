#include "Encoding/Base32.hpp"


namespace Encoding {

Base32Decode::Base32Decode(Mode decodeMode)
		: mDecodeMode{static_cast<std::uint8_t>(decodeMode)}
		, mPrevConsumed{0}
{}

Base32Decode::Base32Decode(Base32Decode&& other) noexcept
{ swap(*this, other); }

void
swap(Base32Decode& a, Base32Decode& b) noexcept
{
	swap(static_cast<Stream::BufferReader&>(a), static_cast<Stream::BufferReader&>(b));
	std::swap(a.mDecodeMode, b.mDecodeMode);
	std::swap(a.mPrevConsumed, b.mPrevConsumed);
	std::swap(a.mPartialDecoded, b.mPartialDecoded);
}

Base32Decode&
Base32Decode::operator=(Base32Decode&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::uint8_t const
DecodeTable[][256]{
{ // STD
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255,  26,  27,  28,  29,  30,  31, 255, 255, 255, 255, 255, 255, 255, 255,
	255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
{ // HEX
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
	255,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
	 25,  26,  27,  28,  29,  30,  31, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
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
Base32Decode::readBytes(std::byte* const dest, std::size_t size)
{
	// provide some data to decode
	auto const prevProduced{::floorz(mPrevConsumed * Base32::DecodedLength, Base32::EncodedLength)}; // to dest
	auto const requested{::ceilz((prevProduced + size) * Base32::EncodedLength, Base32::DecodedLength) - mPrevConsumed};
	
	std::size_t available;
	std::uint8_t const* src;
	if (mDecodeMode & 4) { // padding allowed
		available = getSource().provideSome(::ceilz((prevProduced + size), Base32::DecodedLength) * Base32::EncodedLength - mPrevConsumed); // ask for block aligned data
		src = reinterpret_cast<std::uint8_t const*>(getSource().begin());

		if (mPrevConsumed > 0 && src[0] == '=') { // skip valid padding
			std::size_t padding{1};
			auto const required{Base32::EncodedLength - mPrevConsumed}; // [1, 6]
			while (available < required) {
				while (padding < available) {
					if (src[padding] != '=') [[unlikely]]
						throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};
					++padding;
				}
				available = getSource().provideSomeMore(required - available);
				src = reinterpret_cast<std::uint8_t const*>(getSource().begin()); // the buffer might have changed
			}
			while (padding < required) {
				if (src[padding] != '=') [[unlikely]]
					throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};
				++padding;
			}

			mPrevConsumed = 0;
			getSource().consumed(required);
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
	if (auto const required{::ceilz((prevProduced + 1) * Base32::EncodedLength, Base32::DecodedLength) - mPrevConsumed}; valid < required) {
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

	size  = ::floorz((mPrevConsumed + valid) * Base32::DecodedLength, Base32::EncodedLength) - prevProduced;
	valid = ::ceilz((prevProduced + size) * Base32::EncodedLength, Base32::DecodedLength) - mPrevConsumed;


	// decode
	std::size_t inl{0};
	std::size_t outl{0};

	switch (mPrevConsumed) { // decode incomplete block first
		case 2: {
			dest[outl++] = decodeTable[mPartialDecoded] << 6 | decodeTable[src[inl]] << 1 | decodeTable[src[inl + 1]] >> 4;
			mPrevConsumed += 2;
			mPartialDecoded = src[inl + 1];
			if ((inl += 2) == valid)
				goto afterDecode;
		}
		case 4: {
			dest[outl++] = decodeTable[mPartialDecoded] << 4 | decodeTable[src[inl]] >> 1;
			++mPrevConsumed;
			mPartialDecoded = src[inl];
			if (++inl == valid)
				goto afterDecode;
		}
		case 5: {
			dest[outl++] = decodeTable[mPartialDecoded] << 7 | decodeTable[src[inl]] << 2 | decodeTable[src[inl + 1]] >> 3;
			mPrevConsumed += 2;
			mPartialDecoded = src[inl + 1];
			if ((inl += 2) == valid)
				goto afterDecode;
		}
		case 7: {
			dest[outl++] = decodeTable[mPartialDecoded] << 5 | decodeTable[src[inl++]];
			mPrevConsumed = 0;
		}
	}

	while (valid - inl >= Base32::EncodedLength) { // decode full blocks
		dest[outl    ] = decodeTable[src[inl    ]] << 3 | decodeTable[src[inl + 1]] >> 2;
		dest[outl + 1] = decodeTable[src[inl + 1]] << 6 | decodeTable[src[inl + 2]] << 1 | decodeTable[src[inl + 3]] >> 4;
		dest[outl + 2] = decodeTable[src[inl + 3]] << 4 | decodeTable[src[inl + 4]] >> 1;
		dest[outl + 3] = decodeTable[src[inl + 4]] << 7 | decodeTable[src[inl + 5]] << 2 | decodeTable[src[inl + 6]] >> 3;
		dest[outl + 4] = decodeTable[src[inl + 6]] << 5 | decodeTable[src[inl + 7]]     ;
		outl += Base32::DecodedLength;
		inl  += Base32::EncodedLength;
	}

	if (valid - inl > 1) { // decode remaining partial block
		dest[outl++] = decodeTable[src[inl]] << 3 | decodeTable[src[inl + 1]] >> 2;
		if (valid - inl > 3) {
			dest[outl++] = decodeTable[src[inl + 1]] << 6 | decodeTable[src[inl + 2]] << 1 | decodeTable[src[inl + 3]] >> 4;
			if (valid - inl > 4) {
				dest[outl++] = decodeTable[src[inl + 3]] << 4 | decodeTable[src[inl + 4]] >> 1;
				if (valid - inl > 6) {
					dest[outl++] = decodeTable[src[inl + 4]] << 7 | decodeTable[src[inl + 5]] << 2 | decodeTable[src[inl + 6]] >> 3;
					mPartialDecoded = src[(inl += (mPrevConsumed = 7)) - 1];
				} else
					mPartialDecoded = src[(inl += (mPrevConsumed = 5)) - 1];
			} else
				mPartialDecoded = src[(inl += (mPrevConsumed = 4)) - 1];
		} else
			mPartialDecoded = src[(inl += (mPrevConsumed = 2)) - 1];
	}

	afterDecode:
	if (mDecodeMode & 4 && mPrevConsumed > 0) { // padding allowed
		auto const required{Base32::EncodedLength - mPrevConsumed};
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


Base32Encode::Base32Encode(Mode encodeMode)
		: mEncodeMode{static_cast<std::uint8_t>(encodeMode)}
		, mPrevConsumed{0}
{}

Base32Encode::Base32Encode(Base32Encode&& other) noexcept
{ swap(*this, other); }

void
swap(Base32Encode& a, Base32Encode& b) noexcept
{
	swap(static_cast<Stream::BufferWriter&>(a), static_cast<Stream::BufferWriter&>(b));
	std::swap(a.mEncodeMode, b.mEncodeMode);
	std::swap(a.mPrevConsumed, b.mPrevConsumed);
	std::swap(a.mPartialEncoded, b.mPartialEncoded);
}

Base32Encode&
Base32Encode::operator=(Base32Encode&& other) noexcept
{
	swap(*this, other);
	return *this;
}

Base32Encode::~Base32Encode()
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
EncodeTable[][32]{
{ // STD
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'},
{ // HEX
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'}
};

std::size_t
Base32Encode::writeBytes(std::byte const* s, std::size_t size)
{
	// allocate some space to encode
	auto const prevProduced{::floorz(mPrevConsumed * Base32::EncodedLength, Base32::DecodedLength)}; // from src
	auto const requested{::ceilz((mPrevConsumed + size) * Base32::EncodedLength, Base32::DecodedLength) - prevProduced};

	std::size_t available;
	std::size_t required;
	if (mEncodeMode & 4) { // padding allowed
		available = getSink().allocSome(::ceilz((mPrevConsumed + size), Base32::DecodedLength) * Base32::EncodedLength - prevProduced); // ask for block aligned space
		required = ::ceilz((mPrevConsumed + 1), Base32::DecodedLength) * Base32::EncodedLength - prevProduced;
	} else {
		available = getSink().allocSome(requested); // ask for requested space
		required = ::ceilz((mPrevConsumed + 1) * Base32::EncodedLength, Base32::DecodedLength) - prevProduced;
	}
	if (available < required)
		available = getSink().alloc(required);
	auto* dest{getSink().begin()};

	// find valid output size
	size = ::floorz((prevProduced + std::min(available, requested)) * Base32::DecodedLength, Base32::EncodedLength) - mPrevConsumed;

	// encode
	std::size_t inl{0};
	std::size_t outl{0};
	auto const* const encodeTable{reinterpret_cast<std::byte const*>(EncodeTable[mEncodeMode & 3])}; // alphabet
	auto const* src{reinterpret_cast<std::uint8_t const*>(s)};

	switch (mPrevConsumed) { // encode incomplete block first
		case 1: {
			dest[outl++] = encodeTable[(mPartialEncoded & 0x07) << 2 | src[inl] >> 6];
			dest[outl++] = encodeTable[(src[inl] & 0X3E) >> 1];
			++mPrevConsumed;
			mPartialEncoded = src[inl];
			if (++inl == size)
				goto afterEncode;
		}
		case 2: {
			dest[outl++] = encodeTable[(mPartialEncoded & 0x01) << 4 | src[inl] >> 4];
			++mPrevConsumed;
			mPartialEncoded = src[inl];
			if (++inl == size)
				goto afterEncode;
		}
		case 3: {
			dest[outl++] = encodeTable[(mPartialEncoded & 0x0F) << 1 | src[inl] >> 7];
			dest[outl++] = encodeTable[(src[inl] & 0x7C) >> 2];
			++mPrevConsumed;
			mPartialEncoded = src[inl];
			if (++inl == size)
				goto afterEncode;
		}
		case 4: {
			dest[outl++] = encodeTable[(mPartialEncoded & 0x03) << 3 | src[inl] >> 5];
			dest[outl++] = encodeTable[(src[inl] & 0x1F)];
			mPrevConsumed = 0;
		}
	}

	while (size - inl >= Base32::DecodedLength) { // encode full blocks
		dest[outl    ] = encodeTable[ src[inl    ]         >> 3                    ];
		dest[outl + 1] = encodeTable[(src[inl    ] & 0x07) << 2 | src[inl + 1] >> 6];
		dest[outl + 2] = encodeTable[(src[inl + 1] & 0X3E) >> 1                    ];
		dest[outl + 3] = encodeTable[(src[inl + 1] & 0x01) << 4 | src[inl + 2] >> 4];
		dest[outl + 4] = encodeTable[(src[inl + 2] & 0x0F) << 1 | src[inl + 3] >> 7];
		dest[outl + 5] = encodeTable[(src[inl + 3] & 0x7C) >> 2                    ];
		dest[outl + 6] = encodeTable[(src[inl + 3] & 0x03) << 3 | src[inl + 4] >> 5];
		dest[outl + 7] = encodeTable[ src[inl + 4] & 0x1F                          ];
		outl += Base32::EncodedLength;
		inl  += Base32::DecodedLength;
	}

	if (size - inl > 0) { // encode remaining partial block
		dest[outl++] = encodeTable[src[inl] >> 3];
		if (size - inl > 1) {
			dest[outl++] = encodeTable[(src[inl    ] & 0x07) << 2 | src[inl + 1] >> 6];
			dest[outl++] = encodeTable[(src[inl + 1] & 0X3E) >> 1];
			if (size - inl > 2) {
				dest[outl++] = encodeTable[(src[inl + 1] & 0x01) << 4 | src[inl + 2] >> 4];
				if (size - inl > 3) {
					dest[outl++] = encodeTable[(src[inl + 2] & 0x0F) << 1 | src[inl + 3] >> 7];
					dest[outl++] = encodeTable[(src[inl + 3] & 0x7C) >> 2];
					mPartialEncoded = src[(inl += (mPrevConsumed = 4)) - 1];
				} else
					mPartialEncoded = src[(inl += (mPrevConsumed = 3)) - 1];
			} else
				mPartialEncoded = src[(inl += (mPrevConsumed = 2)) - 1];
		} else
			mPartialEncoded = src[(inl += (mPrevConsumed = 1)) - 1];
	}

	afterEncode:
	getSink().produced(outl);
	return inl;
}

void
Base32Encode::flush()
{
	if (mPrevConsumed > 0) {
		// allocate required space to encode
		std::size_t required;
		if (mEncodeMode & 4) // padding allowed
			getSink().alloc(required = Base32::EncodedLength - ::floorz(mPrevConsumed * Base32::EncodedLength, Base32::DecodedLength)); // ask for block aligned space
		else
			getSink().allocSome(required = 1); // ask for required space
		auto* dest{getSink().begin()};

		// encode remaining bits of last byte
		auto const* const encodeTable{reinterpret_cast<std::byte const*>(EncodeTable[mEncodeMode & 3])}; // alphabet
		switch (mPrevConsumed) {
			case 1: {
				dest[0] = encodeTable[(mPartialEncoded & 0x07) << 2];
			} break;
			case 2: {
				dest[0] = encodeTable[(mPartialEncoded & 0x01) << 4];
			} break;
			case 3: {
				dest[0] = encodeTable[(mPartialEncoded & 0x0F) << 1];
			} break;
			case 4: {
				dest[0] = encodeTable[(mPartialEncoded & 0x03) << 3];
			}
		}
		mPrevConsumed = 0;

		// write padding if required
		for (std::size_t i{1}; i < required; ++i) // padding allowed
			dest[i] = std::byte{'='};
		getSink().produced(required);
	}
}


Base32::Base32(Base32Decode::Mode decodeMode, Base32Encode::Mode encodeMode)
		: Base32Decode{decodeMode}
		, Base32Encode{encodeMode}
{}

void
swap(Base32& a, Base32& b) noexcept
{
	swap(static_cast<Base32Decode&>(a), static_cast<Base32Decode&>(b));
	swap(static_cast<Base32Encode&>(a), static_cast<Base32Encode&>(b));
}

}//namespace Encoding
