#include "Encoding/Base16.hpp"


namespace Encoding {

Base16Decode::Base16Decode(Base16Decode&& other) noexcept
{ swap(*this, other); }

void
swap(Base16Decode& a, Base16Decode& b) noexcept
{ swap(static_cast<Stream::BufferReader&>(a), static_cast<Stream::BufferReader&>(b)); }

Base16Decode&
Base16Decode::operator=(Base16Decode&& other) noexcept
{
	swap(*this, other);
	return *this;
}

std::uint8_t const
DecodeTable[]{
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9, 255, 255, 255, 255, 255, 255,
	255,  10,  11,  12,  13,  14,  15, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255,  10,  11,  12,  13,  14,  15, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

std::size_t
Base16Decode::readBytes(std::byte* const dest, std::size_t size)
{
	// provide some data to decode
	auto available{getSource().provideSome(size * Base16::EncodedLength)};
	auto const* src{reinterpret_cast<std::uint8_t const*>(getSource().begin())};


	// find valid input size
	auto const* const decodeTable{reinterpret_cast<std::byte const*>(DecodeTable)}; // alphabet
	if (decodeTable[src[0]] == std::byte{255}) [[unlikely]]
		throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};
	if (auto const required{Base16::EncodedLength}; available < required) {
		available = getSource().provideSomeMore(1/*required - available*/);
		src = reinterpret_cast<std::uint8_t const*>(getSource().begin()); // the buffer might have changed
	}
	if (decodeTable[src[1]] == std::byte{255}) [[unlikely]]
		throw Exception{std::make_error_code(std::errc::illegal_byte_sequence)};

	std::size_t valid{2};
	for (; valid < available && decodeTable[src[valid]] != std::byte{255}; ++valid);

	size  = ::floorz(valid, Base16::EncodedLength);
	valid = size * Base16::EncodedLength;


	// decode
	std::size_t inl{0};
	std::size_t outl{0};

	while (valid > inl) { // decode full blocks
		dest[outl++] = decodeTable[src[inl]] << 4 | decodeTable[src[inl + 1]];
		inl += Base16::EncodedLength;
	}

	getSource().consumed(inl);
	return outl;
}

Base16Encode::Base16Encode(Base16Encode&& other) noexcept
{ swap(*this, other); }

void
swap(Base16Encode& a, Base16Encode& b) noexcept
{ swap(static_cast<Stream::BufferWriter&>(a), static_cast<Stream::BufferWriter&>(b)); }

Base16Encode&
Base16Encode::operator=(Base16Encode&& other) noexcept
{
	swap(*this, other);
	return *this;
}

/**
 * @see		<a href="https://tools.ietf.org/html/rfc4648#section-3.4">Choosing the Alphabet</a>
 */
std::uint8_t const
EncodeTable[]
{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

std::size_t
Base16Encode::writeBytes(std::byte const* s, std::size_t size)
{
	// allocate some space to encode
	auto available{getSink().allocSome(size * Base16::EncodedLength)};
	if (auto const required{Base16::EncodedLength}; available < required)
		available = getSink().allocSomeMore(1/*required - available*/);
	auto* dest{getSink().begin()};

	size = ::floorz(available, Base16::EncodedLength);

	// encode
	std::size_t inl{0};
	std::size_t outl{0};
	auto const* const encodeTable{reinterpret_cast<std::byte const*>(EncodeTable)}; // alphabet
	auto const* src{reinterpret_cast<std::uint8_t const*>(s)};
	

	while (size > inl) { // decode full blocks
		dest[outl    ] = encodeTable[src[inl] >> 4];
		dest[outl + 1] = encodeTable[src[inl] & 0x0F];
		outl += Base16::EncodedLength;
		inl  += Base16::DecodedLength;
	}

	getSink().produced(outl);
	return inl;
}

void
swap(Base16& a, Base16& b) noexcept
{
	swap(static_cast<Base16Decode&>(a), static_cast<Base16Decode&>(b));
	swap(static_cast<Base16Encode&>(a), static_cast<Base16Encode&>(b));
}

}//namespace Encoding
