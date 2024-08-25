// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TIMESTAMP_HPP
#define NAZARA_CORE_TIMESTAMP_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Serialization.hpp>
#include <Nazara/Core/Time.hpp>
#include <chrono>
#include <cstdlib>
#include <iosfwd>
#include <type_traits>
#include <version>

namespace Nz
{
	class NAZARA_CORE_API Timestamp
	{
		public:
			Timestamp() = default;
			Timestamp(const Timestamp&) = default;
			Timestamp(Timestamp&&) = default;
			~Timestamp() = default;

			constexpr Time AsTime() const;
			constexpr Int64 AsMicroseconds() const;
			constexpr Int64 AsMilliseconds() const;
			constexpr Int64 AsNanoseconds() const;
			constexpr Int64 AsSeconds() const;
#if __cpp_lib_chrono >= 201907L
			template<typename T> constexpr T AsTimepoint() const;
#endif

			constexpr Time GetRemainder() const;

			Timestamp& operator=(const Timestamp&) = default;
			Timestamp& operator=(Timestamp&&) = default;

			constexpr Timestamp& operator+=(Time time);
			constexpr Timestamp& operator-=(Time time);

			constexpr explicit operator Int64() const;

			static constexpr Timestamp Epoch();
			static constexpr Timestamp FromMilliseconds(Int64 milliseconds);
			static constexpr Timestamp FromMicroseconds(Int64 microseconds);
			static constexpr Timestamp FromNanoseconds(Int64 nanoseconds);
			static constexpr Timestamp FromSeconds(Int64 seconds);
			static constexpr Timestamp FromTime(Time time);
#if __cpp_lib_chrono >= 201907L
			template<typename Clock, typename Duration> static constexpr Timestamp FromTimepoint(const std::chrono::time_point<Clock, Duration>& timepoint);
#endif
			static Timestamp Now();

			// External part

			friend constexpr Timestamp operator+(Timestamp lhs, Time rhs);
			friend constexpr Time operator-(Timestamp lhs, Timestamp rhs);

			friend constexpr bool operator==(Timestamp lhs, Timestamp rhs);
			friend constexpr bool operator!=(Timestamp lhs, Timestamp rhs);
			friend constexpr bool operator<(Timestamp lhs, Timestamp rhs);
			friend constexpr bool operator<=(Timestamp lhs, Timestamp rhs);
			friend constexpr bool operator>(Timestamp lhs, Timestamp rhs);
			friend constexpr bool operator>=(Timestamp lhs, Timestamp rhs);

			friend NAZARA_CORE_API std::ostream& operator<<(std::ostream& out, Timestamp time);

			friend inline bool Deserialize(SerializationContext& context, Timestamp* time, TypeTag<Timestamp>);
			friend inline bool Serialize(SerializationContext& context, Timestamp time, TypeTag<Timestamp>);

		private:
			constexpr explicit Timestamp(Int64 nanoseconds);

			Int64 m_nanoseconds;
	};
}

#include <Nazara/Core/Timestamp.inl>

#endif // NAZARA_CORE_TIMESTAMP_HPP
