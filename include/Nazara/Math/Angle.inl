// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Math/Angle.hpp>
#include <algorithm>
#include <cstring>
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

		// Naive implementation, hopefully optimized by the compiler
		template<typename T>
		void SinCos(T x, T* sin, T* cos)
		{
			*sin = std::sin(x);
			*cos = std::cos(x);
		}
	}
	/*!
	* \ingroup math
	* \class Nz::Angle
	* \brief Math class that represents an angle
	*/

	/*!
	* \brief Constructs an Angle object with an angle value
	*
	* \param Angle value of the angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>::Angle(T Angle) :
	angle(Angle)
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
		return std::cos(ToRadians().angle);
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
		return std::sin(ToRadians().angle);
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
		Detail::SinCos<T>(ToRadians().angle, &sin, &cos);

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
		return std::tan(ToRadians().angle);
	}

	/*!
	* \brief Changes the angle value to zero
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>& Angle<Unit, T>::MakeZero()
	{
		angle = T(0);
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
		constexpr T limit = Detail::AngleUtils<Unit>::GetLimit<T>();
		constexpr T twoLimit = limit * T(2);

		angle = std::fmod(angle, twoLimit);
		if (angle < T(0))
			angle += twoLimit;
	}

	/*!
	* \brief Changes the angle value by converting a radian angle
	*
	* \param Angle Radian angle which will be converted
	*/
	template<AngleUnit Unit, typename T>
	template<AngleUnit U, typename>
	Angle<Unit, T>& Angle<Unit, T>::Set(const Angle<AngleUnit::Radian, T>& Angle)
	{
		angle = RadianToDegree(Angle.angle);
		return *this;
	}

	/*!
	* \brief Changes the angle value by converting a degree angle
	*
	* \param Angle Degree angle which will be converted
	*/
	template<AngleUnit Unit, typename T>
	template<AngleUnit U, typename>
	Angle<Unit, T>& Angle<Unit, T>::Set(const Angle<AngleUnit::Degree, T>& Angle)
	{
		angle = DegreeToRadian(Angle.angle);
		return *this;
	}

	/*!
	* \brief Copies the angle value of an angle
	*
	* \param Angle Angle which will be copied
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>& Angle<Unit, T>::Set(const Angle& Angle)
	{
		angle = Angle.angle;
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
	Angle<Unit, T>& Angle<Unit, T>::Set(const Angle<Unit, U>& Angle)
	{
		angle = static_cast<T>(Angle.angle);
		return *this;
	}

	/*!
	* \brief Returns the degree angle that is equivalent to this one
	* \return Equivalent degree angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<AngleUnit::Degree, T> Angle<Unit, T>::ToDegrees() const
	{
		return DegreeAngle<T>(Detail::AngleUtils<Unit>::ToDegrees(angle));
	}

	/*!
	* \brief Returns the radian angle that is equivalent to this angle
	* \return Equivalent radian angle
	*/
	template<AngleUnit Unit, typename T>
	Angle<AngleUnit::Radian, T> Angle<Unit, T>::ToRadians() const
	{
		return RadianAngle<T>(Detail::AngleUtils<Unit>::ToRadians(angle));
	}

	/*!
	* \brief Converts the angle to a string representation
	* \return String representation of the angle
	*/
	template<AngleUnit Unit, typename T>
	String Angle<Unit, T>::ToString() const
	{
		return Detail::AngleUtils<Unit>::ToString(angle);
	}

	/*!
	* \brief Addition operator
	* \return Adds two angles together
	*
	* \param angle Angle to add
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> Angle<Unit, T>::operator+(const Angle& Angle) const
	{
		return Angle(angle + Angle.angle);
	}

	/*!
	* \brief Subtraction operator
	* \return Subtracts two angles together
	*
	* \param angle Angle to subtract
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T> Angle<Unit, T>::operator-(const Angle& Angle) const
	{
		return Angle(angle - Angle.angle);
	}

	/*!
	* \brief Adds an angle by another
	* \return A reference to the angle
	*
	* \param angle Angle to add
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>& Angle<Unit, T>::operator+=(const Angle& Angle)
	{
		angle += Angle.angle;
		return *this;
	}

	/*!
	* \brief Subtract an angle by another
	* \return A reference to the angle
	*
	* \param angle Angle to subtract
	*/
	template<AngleUnit Unit, typename T>
	Angle<Unit, T>& Angle<Unit, T>::operator-=(const Angle& Angle)
	{
		angle -= Angle.angle;
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
		angle *= scalar;
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
		angle /= divider;
		return *this;
	}

	/*!
	* \brief Compares the angle to another for equality
	* \return True if both angles are equal
	*
	* \param Angle The other angle to compare to
	*/
	template<AngleUnit Unit, typename T>
	bool Angle<Unit, T>::operator==(const Angle& Angle) const
	{
		return NumberEquals(angle, Angle.angle, Detail::AngleUtils<Unit>::GetEpsilon<T>());
	}

	/*!
	* \brief Compares the angle to another for inequality
	* \return True if both angles are equal
	*
	* \param Angle The other angle to compare to
	*/
	template<AngleUnit Unit, typename T>
	bool Angle<Unit, T>::operator!=(const Angle& Angle) const
	{
		return !NumberEquals(angle, Angle.angle, Detail::AngleUtils<Unit>::GetEpsilon<T>());
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
		if (!Serialize(context, angle.angle))
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
		if (!Unserialize(context, &angle->angle))
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
	return Nz::Angle<Unit, T>(scale * angle.angle);
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
	return Nz::Angle<Unit, T>(scale / angle.angle);
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
	return Nz::Detail::AngleUtils<Unit>::ToString(out, angle.angle);
}

#include <Nazara/Core/DebugOff.hpp>
