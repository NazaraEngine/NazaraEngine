// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	constexpr Timestamp::Timestamp(Int64 nanoseconds) :
	m_nanoseconds(nanoseconds)
	{
	}

	constexpr Time Timestamp::AsTime() const
	{
		// TODO: Use SafeCast when SafeCast gets constexpr support
		return Time::Nanoseconds(static_cast<Int64>(m_nanoseconds));
	}

	constexpr Int64 Timestamp::AsMicroseconds() const
	{
		return AsTime().AsMicroseconds();
	}

	constexpr Int64 Timestamp::AsMilliseconds() const
	{
		return AsTime().AsMilliseconds();
	}

	constexpr Int64 Timestamp::AsNanoseconds() const
	{
		return AsTime().AsNanoseconds();
	}

	constexpr Int64 Timestamp::AsSeconds() const
	{
		return AsTime().AsSeconds<Int64>();
	}

#if __cpp_lib_chrono >= 201907L
	template<typename T>
	constexpr T Timestamp::AsTimepoint() const
	{
		using Clock = typename T::clock;
		static_assert(std::is_same_v<Clock, std::chrono::system_clock>, "std::chrono::system_clock must be used to represent timestamps using time_point");

		using Duration = typename T::duration;
		if constexpr (std::is_same_v<Duration, std::chrono::nanoseconds>)
			return T(std::chrono::nanoseconds(m_nanoseconds)); //< make sure it's a no-op
		else
		{
			std::chrono::nanoseconds elapsedTime(m_nanoseconds);
			std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timepoint(elapsedTime);
			return std::chrono::time_point_cast<Duration>(timepoint);
		}
	}
#endif

	constexpr Time Timestamp::GetRemainder() const
	{
		Int64 absNs = (m_nanoseconds >= 0) ? m_nanoseconds : -m_nanoseconds;
		// TODO: Use SafeCast when SafeCast gets constexpr support
		return Time::Nanoseconds(static_cast<Int64>(absNs % 1'000'000'000ull));
	}

	constexpr Timestamp& Timestamp::operator+=(Time time)
	{
		Int64 timestamp = m_nanoseconds;
		Int64 ns = time.AsNanoseconds();
		if (ns >= 0)
		{
			timestamp += ns;
			NazaraAssert(timestamp >= m_nanoseconds, "timestamp overflow detected");
		}
		else
		{
			timestamp += ns;
			NazaraAssert(timestamp < m_nanoseconds, "timestamp underflow detected");
		}

		m_nanoseconds = timestamp;
		return *this;
	}

	constexpr Timestamp& Timestamp::operator-=(Time time)
	{
		Int64 timestamp = m_nanoseconds;
		Int64 ns = time.AsNanoseconds();
		if (ns >= 0)
		{
			timestamp -= ns;
			NazaraAssert(timestamp < m_nanoseconds, "timestamp underflow detected");
		}
		else
		{
			timestamp -= ns;
			NazaraAssert(timestamp >= m_nanoseconds, "timestamp overflow detected");
		}

		m_nanoseconds = timestamp;
		return *this;
	}

	constexpr Timestamp::operator Int64() const
	{
		return m_nanoseconds;
	}

	constexpr Timestamp Timestamp::Epoch()
	{
		return Timestamp(0);
	}

	constexpr Timestamp Timestamp::FromMilliseconds(Int64 milliseconds)
	{
		return Timestamp(milliseconds * 1'000'000ull);
	}

	constexpr Timestamp Timestamp::FromMicroseconds(Int64 microseconds)
	{
		return Timestamp(microseconds * 1'000ull);
	}

	constexpr Timestamp Timestamp::FromNanoseconds(Int64 nanoseconds)
	{
		return Timestamp(nanoseconds);
	}

	constexpr Timestamp Timestamp::FromSeconds(Int64 seconds)
	{
		return Timestamp(seconds * 1'000'000'000ull);
	}

	constexpr Timestamp Timestamp::FromTime(Time time)
	{
		return Timestamp(time.AsNanoseconds());
	}

#if __cpp_lib_chrono >= 201907L
	template<typename Clock, typename Duration>
	constexpr Timestamp Timestamp::FromTimepoint(const std::chrono::time_point<Clock, Duration>& timepoint)
	{
		return FromNanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(timepoint.time_since_epoch()).count());
	}
#endif

	constexpr Timestamp operator+(Timestamp lhs, Time rhs)
	{
		Timestamp timestamp(lhs);
		return timestamp += rhs;
	}

	constexpr Time operator-(Timestamp lhs, Timestamp rhs)
	{
		return Time::Nanoseconds(static_cast<Int64>(lhs.m_nanoseconds - rhs.m_nanoseconds));
	}

	constexpr bool operator==(Timestamp lhs, Timestamp rhs)
	{
		return lhs.m_nanoseconds == rhs.m_nanoseconds;
	}

	constexpr bool operator!=(Timestamp lhs, Timestamp rhs)
	{
		return lhs.m_nanoseconds != rhs.m_nanoseconds;
	}

	constexpr bool operator<(Timestamp lhs, Timestamp rhs)
	{
		return lhs.m_nanoseconds < rhs.m_nanoseconds;
	}

	constexpr bool operator<=(Timestamp lhs, Timestamp rhs)
	{
		return lhs.m_nanoseconds <= rhs.m_nanoseconds;
	}

	constexpr bool operator>(Timestamp lhs, Timestamp rhs)
	{
		return lhs.m_nanoseconds > rhs.m_nanoseconds;
	}

	constexpr bool operator>=(Timestamp lhs, Timestamp rhs)
	{
		return lhs.m_nanoseconds >= rhs.m_nanoseconds;
	}

	inline bool Deserialize(SerializationContext& context, Timestamp* timestamp, TypeTag<Timestamp>)
	{
		if (!Deserialize(context, &timestamp->m_nanoseconds))
			return false;

		return true;
	}

	inline bool Serialize(SerializationContext& context, Timestamp timestamp, TypeTag<Timestamp>)
	{
		if (!Serialize(context, timestamp.m_nanoseconds))
			return false;

		return true;
	}
}

