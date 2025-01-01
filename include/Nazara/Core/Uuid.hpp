// Copyright (C) 2025 Full Cycle Games
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_UUID_HPP
#define NAZARA_CORE_UUID_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Serialization.hpp>
#include <array>
#include <functional>
#include <iosfwd>

namespace Nz
{
	class NAZARA_CORE_API Uuid
	{
		public:
			constexpr Uuid();
			constexpr Uuid(const std::array<UInt8, 16>& uuid);
			Uuid(const Uuid&) = default;
			Uuid(Uuid&&) = default;
			~Uuid() = default;

			constexpr bool IsNull() const;

			constexpr const std::array<UInt8, 16>& ToArray() const;
			inline std::string ToString() const;
			std::array<char, 37> ToStringArray() const;

			Uuid& operator=(const Uuid&) = default;
			Uuid& operator=(Uuid&&) = default;

			static constexpr Uuid FromString(std::string_view str);
			static Uuid Generate();

		private:
			std::array<UInt8, 16> m_uuid;
	};

	inline std::ostream& operator<<(std::ostream& out, const Uuid& uuid);
	constexpr bool operator==(const Uuid& lhs, const Uuid& rhs);
	constexpr bool operator!=(const Uuid& lhs, const Uuid& rhs);
	constexpr bool operator<(const Uuid& lhs, const Uuid& rhs);
	constexpr bool operator<=(const Uuid& lhs, const Uuid& rhs);
	constexpr bool operator>(const Uuid& lhs, const Uuid& rhs);
	constexpr bool operator>=(const Uuid& lhs, const Uuid& rhs);

	inline bool Serialize(SerializationContext& context, const Uuid& value, TypeTag<Uuid>);
	inline bool Deserialize(SerializationContext& context, Uuid* value, TypeTag<Uuid>);
}

namespace std
{
	template<>
	struct hash<Nz::Uuid>;
}

#include <Nazara/Core/Uuid.inl>

#endif // NAZARA_CORE_UUID_HPP
