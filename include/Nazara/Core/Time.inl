// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Time.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	constexpr Time::Time(Int64 microseconds) :
	m_nanoseconds(microseconds)
	{
	}

	template<typename T>
	constexpr T Time::AsDuration() const
	{
		if constexpr (std::is_same_v<T, std::chrono::nanoseconds>)
			return std::chrono::nanoseconds(m_nanoseconds); //< make sure it's a no-op
		else
			return std::chrono::duration_cast<T>(std::chrono::nanoseconds(m_nanoseconds));
	}

	template<typename T>
	constexpr T Time::AsSeconds() const
	{
		static_assert(std::is_floating_point_v<T>);
		// TODO: Improve precision
		return AsMicroseconds() / T(1'000'000.0) + (m_nanoseconds % 1000) / T(1'000'000'000);
	}

	constexpr Int64 Time::AsMicroseconds() const
	{
		return m_nanoseconds / 1'000;
	}

	constexpr Int64 Time::AsMilliseconds() const
	{
		return m_nanoseconds / 1'000'000;
	}

	constexpr Int64 Time::AsNanoseconds() const
	{
		return m_nanoseconds;
	}

	constexpr Time& Time::operator+=(Time time)
	{
		m_nanoseconds += time.m_nanoseconds;
		return *this;
	}

	constexpr Time& Time::operator-=(Time time)
	{
		m_nanoseconds -= time.m_nanoseconds;
		return *this;
	}

	constexpr Time& Time::operator*=(Time time)
	{
		m_nanoseconds *= time.m_nanoseconds;
		return *this;
	}

	constexpr Time& Time::operator/=(Time time)
	{
		m_nanoseconds /= time.m_nanoseconds;
		return *this;
	}

	constexpr Time& Time::operator%=(Time time)
	{
		m_nanoseconds %= time.m_nanoseconds;
		return *this;
	}

	constexpr Time::operator Int64() const
	{
		return m_nanoseconds;
	}

	template<class Rep, class Period>
	constexpr Time Time::FromDuration(const std::chrono::duration<Rep, Period>& d)
	{
		return Nanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(d).count());
	}

	constexpr Time Time::Microsecond()
	{
		return Time(1'000);
	}

	constexpr Time Time::Microseconds(Int64 microseconds)
	{
		return Time(microseconds * 1'000);
	}

	constexpr Time Time::Millisecond()
	{
		return Time(1'000'000);
	}

	constexpr Time Time::Milliseconds(Int64 milliseconds)
	{
		return Time(milliseconds * 1'000'000);
	}

	constexpr Time Time::Nanosecond()
	{
		return Time(1);
	}

	constexpr Time Time::Nanoseconds(Int64 nanoseconds)
	{
		return Time(nanoseconds);
	}

	constexpr Time Time::Second()
	{
		return Time(1'000'000'000ull);
	}

	template<typename T>
	constexpr Time Time::Seconds(T seconds)
	{
		if constexpr (std::is_floating_point_v<T>)
			return Nanoseconds(static_cast<UInt64>(seconds * T(1'000'000'000.0)));
		else if constexpr (std::is_integral_v<T>)
			return Nanoseconds(seconds * 1'000'000'000LL);
		else
			static_assert(AlwaysFalse<T>(), "not an arithmetic type");
	}

	constexpr Time Time::TickDuration(Int64 tickRate)
	{
		return Second() / Nanoseconds(tickRate);
	}

	constexpr Time Time::Zero()
	{
		return Time(0);
	}

	constexpr Time operator+(Time time)
	{
		return time;
	}

	constexpr Time operator-(Time time)
	{
		return Time(-time.m_nanoseconds);
	}

	constexpr Time operator+(Time lhs, Time rhs)
	{
		return Time(lhs.m_nanoseconds + rhs.m_nanoseconds);
	}

	constexpr Time operator-(Time lhs, Time rhs)
	{
		return Time(lhs.m_nanoseconds - rhs.m_nanoseconds);
	}

	constexpr Time operator*(Time lhs, Time rhs)
	{
		return Time(lhs.m_nanoseconds * rhs.m_nanoseconds);
	}

	constexpr Time operator/(Time lhs, Time rhs)
	{
		return Time(lhs.m_nanoseconds / rhs.m_nanoseconds);
	}

	constexpr Time operator%(Time lhs, Time rhs)
	{
		return Time(lhs.m_nanoseconds % rhs.m_nanoseconds);
	}

	constexpr bool operator==(Time lhs, Time rhs)
	{
		return lhs.m_nanoseconds == rhs.m_nanoseconds;
	}

	constexpr bool operator!=(Time lhs, Time rhs)
	{
		return lhs.m_nanoseconds != rhs.m_nanoseconds;
	}

	constexpr bool operator<(Time lhs, Time rhs)
	{
		return lhs.m_nanoseconds < rhs.m_nanoseconds;
	}

	constexpr bool operator<=(Time lhs, Time rhs)
	{
		return lhs.m_nanoseconds <= rhs.m_nanoseconds;
	}

	constexpr bool operator>(Time lhs, Time rhs)
	{
		return lhs.m_nanoseconds > rhs.m_nanoseconds;
	}

	constexpr bool operator>=(Time lhs, Time rhs)
	{
		return lhs.m_nanoseconds >= rhs.m_nanoseconds;
	}

	inline std::ostream& operator<<(std::ostream& out, Time time)
	{
		if (time > Time::Second())
			return out << time.AsSeconds<double>() << "s";
		else
		{
			Int64 ns = time.AsNanoseconds();
			if (time > Time::Millisecond())
				return out << ns / 1'000'000.0 << "ms";
			else if (time > Time::Microsecond())
				return out << ns / 1'000.0 << "us";
			else
				return out << ns << "ns";
		}
	}

	inline bool Serialize(SerializationContext& context, Time time, TypeTag<Time>)
	{
		if (!Serialize(context, time.m_nanoseconds))
			return false;

		return true;
	}

	inline bool Unserialize(SerializationContext& context, Time* time, TypeTag<Time>)
	{
		if (!Unserialize(context, &time->m_nanoseconds))
			return false;

		return true;
	}

	namespace Literals
	{
		constexpr Time operator ""_ms(unsigned long long milliseconds)
		{
			return Time::Milliseconds(static_cast<Int64>(milliseconds));
		}

		constexpr Time operator ""_ns(unsigned long long nanoseconds)
		{
			return Time::Nanoseconds(static_cast<Int64>(nanoseconds));
		}

		constexpr Time operator ""_us(unsigned long long microseconds)
		{
			return Time::Microseconds(static_cast<Int64>(microseconds));
		}

		constexpr Time operator ""_s(long double milliseconds)
		{
			return Time::Seconds(milliseconds);
		}

		constexpr Time operator ""_s(unsigned long long milliseconds)
		{
			return Time::Seconds(milliseconds);
		}
	}
}

#include <Nazara/Core/DebugOff.hpp>
