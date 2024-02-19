// Copyright (C) 2024 Full Cycle Games
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	namespace Detail
	{
		constexpr bool ParseHexadecimalPair(Pointer<const char>& str, UInt8& number)
		{
			number = 0;

			for (UInt8 mul : { UInt8(0x10), UInt8(1) })
			{
				if (*str >= '0' && *str <= '9')
					number += (*str - '0') * mul;
				else if (((*str & 0x5F) >= 'A' && (*str & 0x5F) <= 'F'))
					number += ((*str & 0x5F) - 'A' + 10) * mul;
				else
					return false;

				str++;
			}

			return true;
		}
	}

	constexpr Uuid::Uuid() :
	m_uuid{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
	{
	}

	constexpr Uuid::Uuid(const std::array<UInt8, 16>& uuid) :
	m_uuid(uuid)
	{
	}

	constexpr bool Uuid::IsNull() const
	{
		Uuid nullUuid;
		return *this == nullUuid;
	}

	constexpr const std::array<UInt8, 16>& Uuid::ToArray() const
	{
		return m_uuid;
	}

	inline std::string Uuid::ToString() const
	{
		std::array<char, 37> uuidStr = ToStringArray();

		return std::string(uuidStr.data(), uuidStr.size() - 1);
	}

	constexpr Uuid Uuid::FromString(std::string_view str)
	{
		if (str.size() != 36)
			return {};

		const char* ptr = str.data();

		std::array<UInt8, 16> uuid = { 0 };
		UInt8* uuidPart = &uuid[0];

		bool first = true;
		for (std::size_t groupSize : { 4, 2, 2, 2, 6 })
		{
			if (!first && *ptr++ != '-')
				return {};

			first = false;

			for (std::size_t i = 0; i < groupSize; ++i)
			{
				if (!Detail::ParseHexadecimalPair(ptr, *uuidPart++))
					return {};
			}
		}

		return Uuid{ uuid };
	}

	constexpr bool operator==(const Uuid& lhs, const Uuid& rhs)
	{
		const std::array<UInt8, 16>& lhsArray = lhs.ToArray();
		const std::array<UInt8, 16>& rhsArray = rhs.ToArray();
		for (std::size_t i = 0; i < lhsArray.size(); ++i)
		{
			if (lhsArray[i] != rhsArray[i])
				return false;
		}

		return true;
	}

	constexpr bool operator!=(const Uuid& lhs, const Uuid& rhs)
	{
		return !(lhs == rhs);
	}

	constexpr bool operator<(const Uuid& lhs, const Uuid& rhs)
	{
		const std::array<UInt8, 16>& lhsArray = lhs.ToArray();
		const std::array<UInt8, 16>& rhsArray = rhs.ToArray();
		for (std::size_t i = 0; i < lhsArray.size(); ++i)
		{
			if (lhsArray[i] != rhsArray[i])
				return lhsArray[i] < rhsArray[i];
		}

		return false;
	}

	constexpr bool operator<=(const Uuid& lhs, const Uuid& rhs)
	{
		return !(rhs < lhs);
	}

	constexpr bool operator>(const Uuid& lhs, const Uuid& rhs)
	{
		return rhs < lhs;
	}

	constexpr bool operator>=(const Uuid& lhs, const Uuid& rhs)
	{
		return !(lhs < rhs);
	}

	bool Serialize(SerializationContext& context, const Uuid& value, TypeTag<Uuid>)
	{
		const std::array<Nz::UInt8, 16>& array = value.ToArray();
		if (context.stream->Write(array.data(), array.size()) != array.size())
			return false;

		return true;
	}

	bool Unserialize(SerializationContext& context, Uuid* value, TypeTag<Uuid>)
	{
		std::array<Nz::UInt8, 16> array;
		if (context.stream->Read(array.data(), array.size()) != array.size())
			return false;

		*value = Uuid(array);
		return true;
	}

	std::ostream& operator<<(std::ostream& out, const Uuid& guid)
	{
		std::array<char, 37> uuidStr = guid.ToStringArray();

		return out << uuidStr.data();
	}
}

namespace std
{
	template <>
	struct hash<Nz::Uuid>
	{
		size_t operator()(const Nz::Uuid& uuid) const
		{
			// DJB2 algorithm http://www.cse.yorku.ca/~oz/hash.html
			size_t h = 5381;

			const array<Nz::UInt8, 16>& data = uuid.ToArray();
			for (size_t i = 0; i < data.size(); ++i)
				h = ((h << 5) + h) + data[i];

			return h;
		}
	};
}

