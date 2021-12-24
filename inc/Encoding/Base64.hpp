#pragma once

#include <Stream/Buffer.hpp>


namespace Encoding {

/**
 * %Base64 Stream::Input decoder
 * @class	Base64Decode Base64.hpp "Encoding/Base64.hpp"
 */
class Base64Decode : public Stream::BufferReader {

public:

	/**
	 * %Base64 decoding modes
	 * @enum	Mode Base64.hpp "Encoding/Base64.hpp"
	 */
	enum class Mode : std::uint8_t {
		// alphabet_index | padding_allowed | padding_mandatory | line_splitted | ignore_invalid
		STD  = 0 | 4 | 0 |  0 |  0,
		UTF7 = 0 | 0 | 0 |  0 |  0,
		URL  = 1 | 4 | 0 |  0 |  0,
		IMAP = 2 | 0 | 0 |  0 |  0,
	};//enum class Encoding::Base64Decode::Mode
	//PEM  = 0 | 4 | 8 | 16 |  0, // line length 64, or lower for the last line
	//MIME = 0 | 4 | 8 | 16 | 32, // line length at most 76

private:

	std::uint8_t mDecodeMode;
	std::uint8_t mPrevConsumed;
	std::uint8_t mPartialDecoded;

	std::size_t
	readBytes(std::byte* dest, std::size_t size) override;

public:

	explicit
	Base64Decode(Mode decodeMode);

	Base64Decode(Base64Decode&& other) noexcept;

	friend void
	swap(Base64Decode& a, Base64Decode& b) noexcept;

	Base64Decode&
	operator=(Base64Decode&& other) noexcept;

};//class Encoding::Base64Decode


/**
 * %Base64 Stream::Output encoder
 * @class	Base64Encode Base64.hpp "Encoding/Base64.hpp"
 */
class Base64Encode : public Stream::BufferWriter {
public:

	/**
	 * %Base64 encoding modes
	 * @enum	Mode Base64.hpp "Encoding/Base64.hpp"
	 */
	enum class Mode : std::uint8_t {
		// alphabet_index | padding_allowed | padding_mandatory | line_splitted | ignore_invalid
		STD  = 0 | 4 | 0 |  0 |  0,
		UTF7 = 0 | 0 | 0 |  0 |  0,
		URL  = 1 | 4 | 0 |  0 |  0,
		IMAP = 2 | 0 | 0 |  0 |  0,
	};//enum class Encoding::Base64Encode::Mode
	
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
	Base64Encode(Mode encodeMode);

	Base64Encode(Base64Encode&& other) noexcept;

	friend void
	swap(Base64Encode& a, Base64Encode& b) noexcept;

	Base64Encode&
	operator=(Base64Encode&& other) noexcept;

	~Base64Encode();

};//class Encoding::Base64Encode


/**
 * %Base64 Stream::Input decoder and Stream::Output encoder
 * @class	Base64 Base64.hpp "Encoding/Base64.hpp"
 */
class Base64 : public Base64Decode, public Base64Encode {
public:

	static inline constexpr std::uint8_t EncodedLength = 4;
	static inline constexpr std::uint8_t DecodedLength = 3;

	Base64(Base64Decode::Mode decodeMode, Base64Encode::Mode encodeMode);

};//class Encoding::Base64

void
swap(Base64& a, Base64& b) noexcept;

}//namespace Encoding
