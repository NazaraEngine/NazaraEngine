// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Math/Angle.hpp>
#include <algorithm>
#include <cstring>

#ifdef NAZARA_PLATFORM_POSIX
#include <math.h> //< sincos
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<AngleUnit Unit> struct AngleUtils;

		template<>
		struct AngleUtils<AngleUnit::Degree>
		{
			template<typename T> static constexpr T GetEpsilon()
			{
				return T(1e-4);
			}

			template<typename T> static constexpr T GetLimit()
			{
				return 180;
			}

			template<typename T> static T FromDegrees(T degrees)
			{
				return degrees;
			}

			template<typename T> static T FromRadians(T radians)
			{
				return RadianToDegree(radians);
			}

			template<typename T> static T ToDegrees(T degrees)
			{
				return degrees;
			}

			template<typename T> static T ToRadians(T degrees)
			{
				return DegreeToRadian(degrees);
			}

			template<typename T> static String ToString(T value)
			{
				return "Angle(" + String::Number(value) + "deg)";
			}

			template<typename T> static std::ostream& ToString(std::ostream& out, T value)
			{
				return out << "Angle(" << value << "deg)";
			}
		};

		template<>
		struct AngleUtils<AngleUnit::Radian>
		{
			template<typename T> static constexpr T GetEpsilon()
			{
				return T(1e-5);
			}

			template<typename T> static constexpr T GetLimit()
			{
				return T(M_PI);
			}

			template<typename T> static T FromDegrees(T degrees)
			{
				return DegreeToRadian(degrees);
			}

			template<typename T> static T FromRadians(T radians)
			{
				return radians;
			}

			template<typename T> static T ToDegrees(T radians)
			{
				return RadianToDegree(radians);
			}

			template<typename T> static T ToRadians(T radians)
			{
				return radians;
			}

			template<typename T> static String ToString(T value)
			{
				return "Angle(" + String::Number(value) + "rad)";
			}

			template<typename T> static std::ostream& ToString(std::ostream& out, T value)
			{
				return out << "Angle(" << value << "rad)";
			}
		};

#ifdef NAZARA_PLATFORM_POSIX
		template<typename T>
		void SinCos(std::enable_if_t<!std::is_same<T, float>::value && !std::is_same<T, long double>::value, double> x, T* sin, T* cos)
		{
			double s, c;
			::sincos(x, &s, &c);

			*sin = static_cast<T>(s);
			*cos = static_cast<T>(c);
		}

		template<typename T>
		void SinCos(std::enable_if_t<std::is_same<T, float>::value, float> x, float* s, float* c)
		{
			::sincosf(x, s, c);
		}

		template<typename T>
		void SinCos(std::enable_if_t<std::is_same<T, long double>::value, long double> x, long double* s, long double* c)
		{
			::sincosl(x, sin, cos);
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
	* \class Nz::Angle
	* \brief Math class that represents an angle
	*/

	/*!
	* \brief Constructs an Angle object with an angle value
	*
	* \param value value of the angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>::Angle(T angle) :
	value(angle)
	{
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
	* \brief Changes the angle value to zero
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>& Angle<Unit, T>::MakeZero()
	{
		value = T(0);
		return *this;
	}

	/*!
	* \brief Normalizes the angle value
	*
	* If angle exceeds local limits positively or negatively, bring it back between them.
	* For degree angles, local limits are [-180, 180]
	* For radian angles, local limits are [-M_PI, M_PI]
	*/
	template<AngleUnit Unit, typename T>
	void Angle<Unit, T>::Normalize()
	{
		constexpr T limit = Detail::AngleUtils<Unit>::template GetLimit<T>();
		constexpr T twoLimit = limit * T(2);

		value = std::fmod(value, twoLimit);
		if (value < T(0))
			value += twoLimit;
	}

	/*!
	* \brief Copies the angle value of an angle
	*
	* \param Angle Angle which will be copied
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>& Angle<Unit, T>::Set(const Angle& ang)
	{
		value = ang.value;
		return *this;
	}

	/*!
	* \brief Changes the angle value to the same as an Angle of a different type
	*
	* \param Angle Angle which will be casted
	*
	* \remark Conversion from U to T occurs using static_cast
	*/
	template<AngleUnit Unit, typename T>
	template<typename U>
	Angle<Unit, T>& Angle<Unit, T>::Set(const Angle<Unit, U>& ang)
	{
		value = static_cast<T>(ang.value);
		return *this;
	}

	/*!
	* \brief Returns the degree angle that is equivalent to this one
	* \return Equivalent degree angle value
	*/
	template<AngleUnit Unit, typename T>
	T Angle<Unit, T>::ToDegrees() const
	{
		return Detail::AngleUtils<Unit>::ToDegrees(value);
	}

	/*!
	* \brief Returns the degree angle that is equivalent to this one
	* \return Equivalent degree angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<AngleUnit::Degree, T> Angle<Unit, T>::ToDegreeAngle() const
	{
		return DegreeAngle<T>(ToDegrees());
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
	T Angle<Unit, T>::ToRadians() const
	{
		return Detail::AngleUtils<Unit>::ToRadians(value);
	}

	/*!
	* \brief Returns the radian angle that is equivalent to this angle
	* \return Equivalent radian angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<AngleUnit::Radian, T> Angle<Unit, T>::ToRadianAngle() const
	{
		return RadianAngle<T>(ToRadians());
	}

	/*!
	* \brief Converts the angle to a string representation
	* \return String representation of the angle
	*/
	template<AngleUnit Unit, typename T>
	String Angle<Unit, T>::ToString() const
	{
		return Detail::AngleUtils<Unit>::ToString(value);
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
	* \brief Addition operator
	* \return Adds two angles together
	*
	* \param other Angle to add
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> Angle<Unit, T>::operator+(const Angle& other) const
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
	Angle<Unit, T> Angle<Unit, T>::operator-(const Angle& other) const
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
	Angle<Unit, T> Angle<Unit, T>::operator*(T scalar) const
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
	Angle<Unit, T> Angle<Unit, T>::operator/(T divider) const
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
	Angle<Unit, T>& Angle<Unit, T>::operator+=(const Angle& other)
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
	Angle<Unit, T>& Angle<Unit, T>::operator-=(const Angle& other)
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
	Angle<Unit, T>& Angle<Unit, T>::operator*=(T scalar)
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
	Angle<Unit, T>& Angle<Unit, T>::operator/=(T divider)
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
	bool Angle<Unit, T>::operator==(const Angle& other) const
	{
		return NumberEquals(value, other.value, Detail::AngleUtils<Unit>::template GetEpsilon<T>());
	}

	/*!
	* \brief Compares the angle to another for inequality
	* \return True if both angles are equal
	*
	* \param other The other angle to compare to
	*/
	template<AngleUnit Unit, typename T>
	bool Angle<Unit, T>::operator!=(const Angle& other) const
	{
		return !NumberEquals(value, other.value, Detail::AngleUtils<Unit>::template GetEpsilon<T>());
	}

	/*!
	* \brief Builds an Angle instance using a degree angle, converting if needed
	* \return An angle describing the degree angle as Unit
	*
	* \param ang Degree angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> Angle<Unit, T>::FromDegrees(T ang)
	{
		return Angle(Detail::AngleUtils<Unit>::FromDegrees(ang));
	}

	/*!
	* \brief Builds an Angle instance using a radian angle, converting if needed
	* \return An angle describing the radian angle as Unit
	*
	* \param ang Radian angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> Angle<Unit, T>::FromRadians(T ang)
	{
		return Angle(Detail::AngleUtils<Unit>::FromRadians(ang));
	}

	/*!
	* \brief Returns an angle with an angle of zero
	* \return Zero angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> Angle<Unit, T>::Zero()
	{
		Angle angle;
		angle.MakeZero();

		return angle;
	}

	/*!
	* \brief Serializes an Angle
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param angle Input Angle
	*/
	template<AngleUnit Unit, typename T>
	bool Serialize(SerializationContext& context, const Angle<Unit, T>& angle, TypeTag<Angle<Unit, T>>)
	{
		if (!Serialize(context, angle.value))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes an Angle
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param angle Output Angle
	*/
	template<AngleUnit Unit, typename T>
	bool Unserialize(SerializationContext& context, Angle<Unit, T>* angle, TypeTag<Angle<Unit, T>>)
	{
		if (!Unserialize(context, &angle->value))
			return false;

		return true;
	}
}

/*!
* \brief Multiplication operator
* \return An angle corresponding to scale * angle
*
* \param scale Multiplier
* \param angle Angle
*/
template<Nz::AngleUnit Unit, typename T>
Nz::Angle<Unit, T> operator*(T scale, const Nz::Angle<Unit, T>& angle)
{
	return Nz::Angle<Unit, T>(scale * angle.value);
}

/*!
* \brief Division operator
* \return An angle corresponding to scale / angle
*
* \param scale Divisor
* \param angle Angle
*/
template<Nz::AngleUnit Unit, typename T>
Nz::Angle<Unit, T> operator/(T scale, const Nz::Angle<Unit, T>& angle)
{
	return Nz::Angle<Unit, T>(scale / angle.value);
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param box The box to output
*/
template<Nz::AngleUnit Unit, typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Angle<Unit, T>& angle)
{
	return Nz::Detail::AngleUtils<Unit>::ToString(out, angle.value);
}

#include <Nazara/Core/DebugOff.hpp>
