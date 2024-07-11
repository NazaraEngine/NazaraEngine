// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/Constants.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <algorithm>
#include <cstring>
#include <sstream>

#ifdef NAZARA_PLATFORM_POSIX
#include <math.h> //< sincos
#endif


namespace Nz
{
	namespace Detail
	{
		template<AngleUnit From, AngleUnit To> struct AngleConversion;

		template<AngleUnit Unit>
		struct AngleConversion<Unit, Unit>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return angle;
			}
		};

		template<>
		struct AngleConversion<AngleUnit::Degree, AngleUnit::Radian>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return DegreeToRadian(angle);
			}
		};

		template<>
		struct AngleConversion<AngleUnit::Degree, AngleUnit::Turn>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return angle / T(360);
			}
		};

		template<>
		struct AngleConversion<AngleUnit::Radian, AngleUnit::Degree>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return RadianToDegree(angle);
			}
		};

		template<>
		struct AngleConversion<AngleUnit::Radian, AngleUnit::Turn>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return angle / (T(2) * Pi<T>);
			}
		};

		template<>
		struct AngleConversion<AngleUnit::Turn, AngleUnit::Degree>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return angle * T(360);
			}
		};

		template<>
		struct AngleConversion<AngleUnit::Turn, AngleUnit::Radian>
		{
			template<typename T>
			static constexpr T Convert(T angle)
			{
				return angle * T(2) * Pi<T>;
			}
		};

		template<AngleUnit Unit> struct AngleUtils;

		template<>
		struct AngleUtils<AngleUnit::Degree>
		{
			template<typename T>
			static constexpr T GetEpsilon()
			{
				return T(1e-4);
			}

			template<typename T>
			static constexpr T GetLimit()
			{
				return 360;
			}

			template<typename T> static std::ostream& ToString(std::ostream& out, T value)
			{
				return out << "Angle(" << value << "deg)";
			}
		};

		template<>
		struct AngleUtils<AngleUnit::Radian>
		{
			template<typename T>
			static constexpr T GetEpsilon()
			{
				return T(1e-5);
			}

			template<typename T>
			static constexpr T GetLimit()
			{
				return T(2) * Pi<T>;
			}

			template<typename T>
			static std::ostream& ToString(std::ostream& out, T value)
			{
				return out << "Angle(" << value << "rad)";
			}
		};

		template<>
		struct AngleUtils<AngleUnit::Turn>
		{
			template<typename T>
			static constexpr T GetEpsilon()
			{
				return T(1e-5);
			}

			template<typename T>
			static constexpr T GetLimit()
			{
				return 1;
			}

			template<typename T>
			static std::ostream& ToString(std::ostream& out, T value)
			{
				return out << "Angle(" << value << "turn)";
			}
		};

#ifdef NAZARA_PLATFORM_LINUX
		template<typename T>
		void SinCos(T x, T* sin, T* cos)
		{
			double s, c;
			::sincos(x, &s, &c);

			*sin = static_cast<T>(s);
			*cos = static_cast<T>(c);
		}

		template<>
		inline void SinCos(float x, float* s, float* c)
		{
			::sincosf(x, s, c);
		}

		template<>
		inline void SinCos(long double x, long double* s, long double* c)
		{
			::sincosl(x, s, c);
		}
#else
		// Naive implementation, hopefully optimized by the compiler
		template<typename T>
		void SinCos(T x, T* sin, T* cos)
		{
			*sin = std::sin(x);
			*cos = std::cos(x);
		}
#endif
	}
	/*!
	* \ingroup math
	* \class Angle
	* \brief Math class that represents an angle
	*/

	/*!
	* \brief Constructs an Angle object with an angle value
	*
	* \param value value of the angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T>::Angle(T angle) :
	value(angle)
	{
	}

	template<AngleUnit Unit, typename T>
	template<typename U>
	constexpr Angle<Unit, T>::Angle(const Angle<Unit, U>& angle) :
	value(static_cast<T>(angle.value))
	{
	}

	/*!
	* \brief Constructs an Angle object from a angle in a specific unit, converting if required
	*
	* \param value Angle object to copy
	*/
	template<AngleUnit Unit, typename T>
	template<AngleUnit FromUnit>
	constexpr Angle<Unit, T>::Angle(const Angle<FromUnit, T>& angle) :
	value(Detail::AngleConversion<FromUnit, Unit>::Convert(angle.value))
	{
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::ApproxEqual(const Angle& angle) const
	{
		return ApproxEqual(angle, Detail::AngleUtils<Unit>::template GetEpsilon<T>());
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::ApproxEqual(const Angle& angle, T maxDifference) const
	{
		return NumberEquals(value, angle.value, maxDifference);
	}

	/*!
	* \brief Computes the cosine of the angle
	* \return Cosine of angle
	*
	* \see GetSinCos
	*/
	template<AngleUnit Unit, typename T>
	T Angle<Unit, T>::GetCos() const
	{
		return std::cos(ToRadians());
	}

	/*!
	* \brief Computes the sine of the angle
	* \return Sine of angle
	*
	* \see GetSinCos
	*/
	template<AngleUnit Unit, typename T>
	T Angle<Unit, T>::GetSin() const
	{
		return std::sin(ToRadians());
	}

	/*!
	* \brief Computes both sines and cosines of the angle
	* \return Sine and cosine of the angle
	*
	* \remark This is potentially faster than calling both GetSin and GetCos separately as it can computes both values at the same time.
	*
	* \see GetCos, GetSin
	*/
	template<AngleUnit Unit, typename T>
	std::pair<T, T> Angle<Unit, T>::GetSinCos() const
	{
		T sin, cos;
		Detail::SinCos<T>(ToRadians(), &sin, &cos);

		return std::make_pair(sin, cos);
	}

	/*!
	* \brief Computes the tangent of the angle
	* \return Tangent value of the angle
	*
	* \see GetCos, GetSin
	*/
	template<AngleUnit Unit, typename T>
	T Angle<Unit, T>::GetTan() const
	{
		return std::tan(ToRadians());
	}

	/*!
	* \brief Normalizes the angle value
	*
	* If angle exceeds local limits positively or negatively, bring it back between them.
	* For degree angles, local limits are [-180, 180]
	* For radian angles, local limits are [-Pi, Pi]
	* For turn angles, local limits are [-1, 1]
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T>& Angle<Unit, T>::Normalize()
	{
		constexpr T limit = Detail::AngleUtils<Unit>::template GetLimit<T>();
		constexpr T halfLimit = limit / T(2);

		value = Nz::Mod(value + halfLimit, limit);
		if (value < T(0))
			value += limit;

		value -= halfLimit;
		return *this;
	}

	/*!
	* \brief Returns the ToUnit angle that is equivalent to this one
	* \return Equivalent ToUnit angle value
	*/
	template<AngleUnit Unit, typename T>
	template<AngleUnit ToUnit>
	T Angle<Unit, T>::To() const
	{
		return Detail::AngleConversion<Unit, ToUnit>::Convert(value);
	}

	/*!
	* \brief Returns the ToUnit angle that is equivalent to this one
	* \return Equivalent ToUnit angle
	*/
	template<AngleUnit Unit, typename T>
	template<AngleUnit ToUnit>
	Angle<ToUnit, T> Angle<Unit, T>::ToAngle() const
	{
		return Angle<ToUnit, T>(To<ToUnit>());
	}

	/*!
	* \brief Returns the degree angle that is equivalent to this one
	* \return Equivalent degree angle value
	*/
	template<AngleUnit Unit, typename T>
	constexpr T Angle<Unit, T>::ToDegrees() const
	{
		return To<AngleUnit::Degree>();
	}

	/*!
	* \brief Returns the degree angle that is equivalent to this one
	* \return Equivalent degree angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<AngleUnit::Degree, T> Angle<Unit, T>::ToDegreeAngle() const
	{
		return ToAngle<AngleUnit::Degree>();
	}

	/*!
	* \brief Converts the angle to an Euler Angles representation
	* \return A 2D rotation expressed in Euler angles
	*
	* This will assume two-dimensional usage, and will set the angle value (as degrees) as the roll value of the Euler Angles, leaving pitch and yaw to zero
	*/
	template<AngleUnit Unit, typename T>
	EulerAngles<T> Angle<Unit, T>::ToEulerAngles() const
	{
		return EulerAngles<T>(0, 0, ToDegrees());
	}

	/*!
	* \brief Converts the angle to a Quaternion representation
	* \return A 2D rotation expressed with Quaternion
	*
	* This will assume two-dimensional usage, as if the angle was first converted to Euler Angles and then to a Quaternion
	*
	* \see ToEulerAngles
	*/
	template<AngleUnit Unit, typename T>
	Quaternion<T> Angle<Unit, T>::ToQuaternion() const
	{
		auto halfAngle = Angle(*this) / 2.f;
		auto sincos = halfAngle.GetSinCos();
		return Quaternion<T>(sincos.second, 0, 0, sincos.first);
	}

	/*!
	* \brief Returns the radian angle that is equivalent to this angle
	* \return Equivalent radian angle value
	*/
	template<AngleUnit Unit, typename T>
	constexpr T Angle<Unit, T>::ToRadians() const
	{
		return To<AngleUnit::Radian>();
	}

	/*!
	* \brief Returns the radian angle that is equivalent to this angle
	* \return Equivalent radian angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<AngleUnit::Radian, T> Angle<Unit, T>::ToRadianAngle() const
	{
		return ToAngle<AngleUnit::Radian>();
	}

	/*!
	* \brief Converts the angle to a string representation
	* \return String representation of the angle
	*/
	template<AngleUnit Unit, typename T>
	std::string Angle<Unit, T>::ToString() const
	{
		std::ostringstream oss;
		Detail::AngleUtils<Unit>::ToString(oss, value);

		return oss.str();
	}

	/*!
	* \brief Returns the turn angle that is equivalent to this angle
	* \return Equivalent turn angle value
	*/
	template<AngleUnit Unit, typename T>
	constexpr T Angle<Unit, T>::ToTurns() const
	{
		return To<AngleUnit::Turn>(value);
	}

	/*!
	* \brief Returns the turn angle that is equivalent to this angle
	* \return Equivalent turn angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<AngleUnit::Turn, T> Angle<Unit, T>::ToTurnAngle() const
	{
		return ToAngle<AngleUnit::Turn>();
	}

	/*!
	* \brief Returns the degree angle that is equivalent to this one
	* \return Equivalent degree angle
	*/
	/*template<AngleUnit Unit, typename T>
	template<AngleUnit U, typename>
	Angle<Unit, T>::operator Angle<AngleUnit::Degree, T>() const
	{
		return ToDegreeAngle();
	}*/

	/*!
	* \brief Converts the angle to a string representation
	* \return String representation of the angle
	*/
	/*template<AngleUnit Unit, typename T>
	template<AngleUnit U, typename>
	Angle<Unit, T>::operator Angle<AngleUnit::Radian, T>() const
	{
		return ToRadianAngle();
	}*/


	/*!
	* \brief Helps to represent the sign of the angle
	* \return A constant reference to this angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::operator+() const
	{
		return *this;
	}

	/*!
	* \brief Negates the angle
	* \return An angle with a negated value
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::operator-() const
	{
		return Angle(-value);
	}

	/*!
	* \brief Addition operator
	* \return Adds two angles together
	*
	* \param other Angle to add
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::operator+(Angle other) const
	{
		return Angle(value + other.value);
	}

	/*!
	* \brief Subtraction operator
	* \return Subtracts two angles together
	*
	* \param other Angle to subtract
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::operator-(Angle other) const
	{
		return Angle(value - other.value);
	}

	/*!
	* \brief Multiplication operator
	* \return A copy of the angle, scaled by the multiplier
	*
	* \param scalar Multiplier
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::operator*(T scalar) const
	{
		return Angle(value * scalar);
	}

	/*!
	* \brief Divides the angle by a scalar
	* \return A copy of the angle, divided by the divider
	*
	* \param divider Divider
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::operator/(T divider) const
	{
		return Angle(value / divider);
	}

	/*!
	* \brief Adds an angle by another
	* \return A reference to the angle
	*
	* \param other Angle to add
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T>& Angle<Unit, T>::operator+=(Angle other)
	{
		value += other.value;
		return *this;
	}

	/*!
	* \brief Subtract an angle by another
	* \return A reference to the angle
	*
	* \param other Angle to subtract
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T>& Angle<Unit, T>::operator-=(Angle other)
	{
		value -= other.value;
		return *this;
	}

	/*!
	* \brief Scales the angle by a scalar
	* \return A reference to the angle
	*
	* \param scalar Multiplier
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T>& Angle<Unit, T>::operator*=(T scalar)
	{
		value *= scalar;
		return *this;
	}

	/*!
	* \brief Divides the angle by a scalar
	* \return A reference to the angle
	*
	* \param divider Divider
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T>& Angle<Unit, T>::operator/=(T divider)
	{
		value /= divider;
		return *this;
	}

	/*!
	* \brief Compares the angle to another for equality
	* \return True if both angles are equal
	*
	* \param other The other angle to compare to
	*/
	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::operator==(Angle other) const
	{
		return value == other.value;
	}

	/*!
	* \brief Compares the angle to another for inequality
	* \return True if both angles are equal
	*
	* \param other The other angle to compare to
	*/
	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::operator!=(Angle other) const
	{
		return value != other.value;
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::operator<(Angle other) const
	{
		return value < other.value;
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::operator<=(Angle other) const
	{
		return value <= other.value;
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::operator>(Angle other) const
	{
		return value > other.value;
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::operator>=(Angle other) const
	{
		return value >= other.value;
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::ApproxEqual(const Angle& lhs, const Angle& rhs)
	{
		return lhs.ApproxEqual(rhs);
	}

	template<AngleUnit Unit, typename T>
	constexpr bool Angle<Unit, T>::ApproxEqual(const Angle& lhs, const Angle& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::Clamp(Angle angle, Angle min, Angle max)
	{
		return Angle(std::clamp(angle.value, min.value, max.value));
	}

	/*!
	* \brief Builds an Angle instance using a FromUnit angle, converting if needed
	* \return An angle describing the FromUnit angle as Unit
	*
	* \param ang Degree angle
	*/
	template<AngleUnit Unit, typename T>
	template<AngleUnit FromUnit>
	constexpr Angle<Unit, T> Angle<Unit, T>::From(T value)
	{
		return Angle(Detail::AngleConversion<FromUnit, Unit>::Convert(value));
	}

	/*!
	* \brief Builds an Angle instance using a degree angle, converting if needed
	* \return An angle describing the degree angle as Unit
	*
	* \param ang Degree angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::FromDegrees(T degrees)
	{
		return From<AngleUnit::Degree>(degrees);
	}

	/*!
	* \brief Builds an Angle instance using a radian angle, converting if needed
	* \return An angle describing the radian angle as Unit
	*
	* \param ang Radian angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::FromRadians(T radians)
	{
		return From<AngleUnit::Radian>(radians);
	}

	/*!
	* \brief Builds an Angle instance using a radian angle, converting if needed
	* \return An angle describing the radian angle as Unit
	*
	* \param ang Radian angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::FromTurns(T turns)
	{
		return From<AngleUnit::Turn>(turns);
	}

	/*!
	* \brief Returns an angle with an angle of zero
	* \return Zero angle
	*/
	template<AngleUnit Unit, typename T>
	constexpr Angle<Unit, T> Angle<Unit, T>::Zero()
	{
		return Angle(0);
	}

	/*!
	* \brief Multiplication operator
	* \return An angle corresponding to scale * angle
	*
	* \param scale Multiplier
	* \param angle Angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> operator*(T scale, Angle<Unit, T> angle)
	{
		return Angle<Unit, T>(scale * angle.value);
	}

	/*!
	* \brief Division operator
	* \return An angle corresponding to scale / angle
	*
	* \param scale Divisor
	* \param angle Angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> operator/(T scale, Angle<Unit, T> angle)
	{
		return Angle<Unit, T>(scale / angle.value);
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param box The box to output
	*/
	template<AngleUnit Unit, typename T>
	std::ostream& operator<<(std::ostream& out, Angle<Unit, T> angle)
	{
		return Detail::AngleUtils<Unit>::ToString(out, angle.value);
	}

	/*!
	* \ingroup math
	* \brief Clamps an angle value between min and max and returns the expected value
	* \return If value is not in the interval of min..max, value obtained is the nearest limit of this interval
	*
	* \param value Value to clamp
	* \param min Minimum of the interval
	* \param max Maximum of the interval
	*/
	template<typename T, AngleUnit Unit>
	constexpr Angle<Unit, T> Clamp(Angle<Unit, T> value, T min, T max)
	{
		return std::max(std::min(value.value, max), min);
	}

	/*!
	* \brief Serializes an Angle
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param angle Input Angle
	*/
	template<AngleUnit Unit, typename T>
	bool Serialize(SerializationContext& context, Angle<Unit, T> angle, TypeTag<Angle<Unit, T>>)
	{
		if (!Serialize(context, angle.value))
			return false;

		return true;
	}

	/*!
	* \brief Deserializes an Angle
	* \return true if successfully deserialized
	*
	* \param context Serialization context
	* \param angle Output Angle
	*/
	template<AngleUnit Unit, typename T>
	bool Deserialize(SerializationContext& context, Angle<Unit, T>* angle, TypeTag<Angle<Unit, T>>)
	{
		if (!Deserialize(context, &angle->value))
			return false;

		return true;
	}
}

