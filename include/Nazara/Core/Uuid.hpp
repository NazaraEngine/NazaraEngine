// Copyright (C) 2022 Full Cycle Games
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_UUID_HPP
#define NAZARA_CORE_UUID_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Config.hpp>
#include <array>
#include <functional>
#include <iosfwd>

namespace Nz
{
	class NAZARA_CORE_API Uuid
	{
		public:
			inline Uuid();
			inline Uuid(const std::array<UInt8, 16> uuid);
			Uuid(const Uuid&) = default;
			Uuid(Uuid&& generator) = default;
			~Uuid() = default;

			inline bool IsNull() const;

			inline const std::array<UInt8, 16>& ToArray() const;
			inline std::string ToString() const;
			std::array<char, 37> ToStringArray() const;

			Uuid& operator=(const Uuid&) = default;
			Uuid& operator=(Uuid&&) = default;

			static Uuid FromString(std::string_view str);
			static Uuid Generate();

		private:
			std::array<UInt8, 16> m_uuid;
	};

	NAZARA_CORE_API std::ostream& operator<<(std::ostream& out, const Uuid& uuid);
	inline bool operator==(const Uuid& lhs, const Uuid& rhs);
	inline bool operator!=(const Uuid& lhs, const Uuid& rhs);
	inline bool operator<(const Uuid& lhs, const Uuid& rhs);
	inline bool operator<=(const Uuid& lhs, const Uuid& rhs);
	inline bool operator>(const Uuid& lhs, const Uuid& rhs);
	inline bool operator>=(const Uuid& lhs, const Uuid& rhs);
}

namespace Nz
{
	inline bool Serialize(SerializationContext& context, const Uuid& value, TypeTag<Uuid>);
	inline bool Unserialize(SerializationContext& context, Uuid* value, TypeTag<Uuid>);
}

namespace std
{
	template<>
	struct hash<Nz::Uuid>;
}

#include <Nazara/Core/Uuid.inl>

#endif // NAZARA_CORE_UUID_HPP
