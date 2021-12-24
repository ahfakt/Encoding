#pragma once

#include <Stream/Buffer.hpp>


namespace Encoding {

/**
 * %Base32 Stream::Input decoder
 * @class	Base32Decode Base32.hpp "Encoding/Base32.hpp"
 */
class Base32Decode : public Stream::BufferReader {

public:

	/**
	 * @Base32 decoding modes
	 * @enum	Mode Base32.hpp "Encoding/Base32.hpp"
	 */
	enum class Mode : std::uint8_t {
		// alphabet_index | padding_allowed | padding_mandatory | line_splitted | ignore_invalid
		STD  = 0 | 4 | 0 |  0 |  0,
		STDN = 0 | 0 | 0 |  0 |  0,
		HEX  = 1 | 4 | 0 |  0 |  0,
		HEXN = 1 | 0 | 0 |  0 |  0,
	};//enum class Encoding::Base32Decode::Mode

private:

	std::uint8_t mDecodeMode;
	std::uint8_t mPrevConsumed;
	std::uint8_t mPartialDecoded;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:

	explicit
	Base32Decode(Mode decodeMode);

	Base32Decode(Base32Decode&& other) noexcept;

	friend void
	swap(Base32Decode& a, Base32Decode& b) noexcept;

	Base32Decode&
	operator=(Base32Decode&& other) noexcept;

};//class Encoding::Base32Decode


/**
 * %Base32 Stream::Output encoder
 * @class	Base32Encode Base32.hpp "Encoding/Base32.hpp"
 */
class Base32Encode : public Stream::BufferWriter {
public:

	/**
	 * @Base32 encoding modes
	 * @enum	Mode Base32.hpp "Encoding/Base32.hpp"
	 */
	enum class Mode : std::uint8_t {
		// alphabet_index | padding_allowed | padding_mandatory | line_splitted | ignore_invalid
		STD	 = 0 | 4 | 0 |  0 |  0,
		STDN = 0 | 0 | 0 |  0 |  0,
		HEX	 = 1 | 4 | 0 |  0 |  0,
		HEXN = 1 | 0 | 0 |  0 |  0,
	};//enum class Encoding::Base32Encode::Mode
	
private:

	std::uint8_t mEncodeMode;
	std::uint8_t mPrevConsumed;
	std::uint8_t mPartialEncoded;

	std::size_t
	writeBytes(std::byte const* src, std::size_t size) override;

	void
	flush() override;

public:

	explicit
	Base32Encode(Mode encodeMode);

	Base32Encode(Base32Encode&& other) noexcept;

	friend void
	swap(Base32Encode& a, Base32Encode& b) noexcept;

	Base32Encode&
	operator=(Base32Encode&& other) noexcept;

	~Base32Encode();

};//class Encoding::Base32Encode


/**
 * %Base32 Stream::Input decoder and Stream::Output encoder
 * @class	Base32 Base32.hpp "Encoding/Base32.hpp"
 */
class Base32 : public Base32Decode, public Base32Encode {
public:

	static inline constexpr std::uint8_t EncodedLength = 8;
	static inline constexpr std::uint8_t DecodedLength = 5;

	Base32(Base32Decode::Mode decodeMode, Base32Encode::Mode encodeMode);

};//class Encoding::Base32

void
swap(Base32& a, Base32& b) noexcept;

}//namespace Encoding
