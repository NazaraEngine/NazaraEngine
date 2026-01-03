// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_TIME_HPP
#define NAZARA_CORE_TIME_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Core/Serialization.hpp>
#include <chrono>
#include <iosfwd>
#include <type_traits>

namespace Nz
{
	class Time
	{
		public:
			Time() = default;
			Time(const Time&) = default;
			Time(Time&&) = default;
			~Time() = default;

			template<typename T> constexpr T AsDuration() const;
			template<typename T = float> constexpr T AsSeconds() const;
			constexpr Int64 AsMicroseconds() const;
			constexpr Int64 AsMilliseconds() const;
			constexpr Int64 AsNanoseconds() const;

			Time& operator=(const Time&) = default;
			Time& operator=(Time&&) = default;

			constexpr Time& operator+=(Time time);
			constexpr Time& operator-=(Time time);
			constexpr Time& operator*=(Time time);
			constexpr Time& operator/=(Time time);
			constexpr Time& operator%=(Time time);

			constexpr explicit operator Int64() const;

			template<class Rep, class Period> static constexpr Time FromDuration(const std::chrono::duration<Rep, Period>& duration);
			static constexpr Time Microsecond();
			static constexpr Time Microseconds(Int64 microseconds);
			static constexpr Time Millisecond();
			static constexpr Time Milliseconds(Int64 milliseconds);
			static constexpr Time Nanosecond();
			static constexpr Time Nanoseconds(Int64 nanoseconds);
			static constexpr Time Second();
			template<typename T> static constexpr Time Seconds(T seconds);
			static constexpr Time TickDuration(Int64 tickRate);
			static constexpr Time Zero();

			// External part

			friend constexpr Time operator+(Time time);
			friend constexpr Time operator-(Time time);

			friend constexpr Time operator+(Time lhs, Time rhs);
			friend constexpr Time operator-(Time lhs, Time rhs);
			friend constexpr Time operator*(Time lhs, Time rhs);
			friend constexpr Time operator/(Time lhs, Time rhs);
			friend constexpr Time operator%(Time lhs, Time rhs);

			friend constexpr bool operator==(Time lhs, Time rhs);
			friend constexpr bool operator!=(Time lhs, Time rhs);
			friend constexpr bool operator<(Time lhs, Time rhs);
			friend constexpr bool operator<=(Time lhs, Time rhs);
			friend constexpr bool operator>(Time lhs, Time rhs);
			friend constexpr bool operator>=(Time lhs, Time rhs);

			friend NAZARA_CORE_API std::ostream& operator<<(std::ostream& out, Time time);

			friend inline bool Deserialize(SerializationContext& context, Time* time, TypeTag<Time>);
			friend inline bool Serialize(SerializationContext& context, Time time, TypeTag<Time>);

		private:
			constexpr explicit Time(Int64 nanoseconds);

			Int64 m_nanoseconds;
	};

	namespace Literals
	{
		constexpr Time operator ""_ms(unsigned long long milliseconds);
		constexpr Time operator ""_ns(unsigned long long nanoseconds);
		constexpr Time operator ""_us(unsigned long long microseconds);
		constexpr Time operator ""_s(long double seconds);
		constexpr Time operator ""_s(unsigned long long seconds);
	}

	using GetElapsedTimeFunction = Time(*)();

	extern NAZARA_CORE_API GetElapsedTimeFunction GetElapsedMilliseconds;
	extern NAZARA_CORE_API GetElapsedTimeFunction GetElapsedNanoseconds;
}

#include <Nazara/Core/Time.inl>

#endif // NAZARA_CORE_TIME_HPP
