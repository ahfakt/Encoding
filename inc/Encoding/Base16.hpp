#pragma once

#include <Stream/Buffer.hpp>


namespace Encoding {

/**
 * %Base16 Stream::Input decoder
 * @class	Base16Decode Base16.hpp "Encoding/Base16.hpp"
 */
class Base16Decode : public Stream::BufferReader {
	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:

	Base16Decode() = default;

	Base16Decode(Base16Decode&& other) noexcept;

	friend void
	swap(Base16Decode& a, Base16Decode& b) noexcept;

	Base16Decode&
	operator=(Base16Decode&& other) noexcept;

};//class Encoding::Base16Decode


/**
 * %Base16 Stream::Output encoder
 * @class	Base16Encode Base16.hpp "Encoding/Base16.hpp"
 */
class Base16Encode : public Stream::BufferWriter {

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

public:

	Base16Encode() = default;

	Base16Encode(Base16Encode&& other) noexcept;

	friend void
	swap(Base16Encode& a, Base16Encode& b) noexcept;

	Base16Encode&
	operator=(Base16Encode&& other) noexcept;

};//class Encoding::Base16Encode


/**
 * %Base16 Stream::Input decoder and Stream::Output encoder
 * @class	Base16 Base16.hpp "Encoding/Base16.hpp"
 */
class Base16 : public Base16Decode, public Base16Encode {
public:

	static inline constexpr std::uint8_t EncodedLength = 2;
	static inline constexpr std::uint8_t DecodedLength = 1;

	Base16() = default;

};//class Encoding::Base16

void
swap(Base16& a, Base16& b) noexcept;

}//namespace Encoding
