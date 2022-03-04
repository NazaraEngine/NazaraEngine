// Copyright (C) 2022 Full Cycle Games
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Uuid.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline Uuid::Uuid()
	{
		m_guid.fill(0);
	}

	inline Uuid::Uuid(const std::array<UInt8, 16> guid) :
	m_guid(guid)
	{
	}

	inline bool Uuid::IsNull() const
	{
		Uuid NullGuid;
		return *this == NullGuid;
	}

	inline const std::array<UInt8, 16>& Uuid::ToArray() const
	{
		return m_guid;
	}

	inline std::string Uuid::ToString() const
	{
		std::array<char, 37> guidStr = ToStringArray();

		return std::string(guidStr.data(), guidStr.size() - 1);
	}

	bool operator==(const Uuid& lhs, const Uuid& rhs)
	{
		return lhs.ToArray() == rhs.ToArray();
	}

	bool operator!=(const Uuid& lhs, const Uuid& rhs)
	{
		return lhs.ToArray() != rhs.ToArray();
	}

	bool operator<(const Uuid& lhs, const Uuid& rhs)
	{
		return lhs.ToArray() < rhs.ToArray();
	}

	bool operator<=(const Uuid& lhs, const Uuid& rhs)
	{
		return lhs.ToArray() <= rhs.ToArray();
	}

	bool operator>(const Uuid& lhs, const Uuid& rhs)
	{
		return lhs.ToArray() > rhs.ToArray();
	}

	bool operator>=(const Uuid& lhs, const Uuid& rhs)
	{
		return lhs.ToArray() >= rhs.ToArray();
	}
}

namespace Nz
{
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
}

namespace std
{
	template <>
	struct hash<Nz::Uuid>
	{
		size_t operator()(const Nz::Uuid& guid) const
		{
			// DJB2 algorithm http://www.cse.yorku.ca/~oz/hash.html
			size_t h = 5381;

			const array<Nz::UInt8, 16>& data = guid.ToArray();
			for (size_t i = 0; i < data.size(); ++i)
				h = ((h << 5) + h) + data[i];

			return h;
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
