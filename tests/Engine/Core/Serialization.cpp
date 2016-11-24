#include <Nazara/Core/SerializationContext.hpp>

#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/MemoryView.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Ray.hpp>
#include <array>

#include <Catch/catch.hpp>

SCENARIO("Serialization", "[CORE][SERIALIZATION]")
{
	GIVEN("A context of serialization")
	{
		std::array<char, 256> datas; // The array must be bigger than any of the serializable classes
		Nz::MemoryView stream(datas.data(), datas.size());

		Nz::SerializationContext context;
		context.stream = &stream;

		WHEN("We serialize basic types")
		{
			THEN("Arithmetical types")
			{
				context.stream->SetCursorPos(0);
				REQUIRE(Serialize(context, 3));
				int value = 0;
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &value));
				REQUIRE(value == 3);
			}

			THEN("Boolean type")
			{
				context.stream->SetCursorPos(0);
				REQUIRE(Serialize(context, true));
				context.FlushBits(); //< Don't forget to flush bits (it is NOT done by the stream)
				context.stream->SetCursorPos(0);
				bool value = false;
				REQUIRE(Unserialize(context, &value));
				REQUIRE(value == true);
			}
		}

		WHEN("We serialize mathematical classes")
		{
			THEN("BoudingVolume")
			{
				context.stream->SetCursorPos(0);
				Nz::BoundingVolumef nullVolume = Nz::BoundingVolumef::Null();
				Nz::BoundingVolumef copy(nullVolume);
				REQUIRE(Serialize(context, nullVolume));
				nullVolume = Nz::BoundingVolumef::Infinite();
				REQUIRE(nullVolume != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &nullVolume));
				REQUIRE(nullVolume == copy);
			}

			THEN("Box")
			{
				context.stream->SetCursorPos(0);
				Nz::Boxf zeroBox = Nz::Boxf::Zero();
				Nz::Boxf copy(zeroBox);
				REQUIRE(Serialize(context, zeroBox));
				zeroBox = Nz::Boxf(1, 1, 1, 1, 1, 1);
				REQUIRE(zeroBox != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &zeroBox));
				REQUIRE(zeroBox == copy);
			}

			THEN("EulerAngles")
			{
				context.stream->SetCursorPos(0);
				Nz::EulerAnglesf zeroEuler = Nz::EulerAnglesf::Zero();
				Nz::EulerAnglesf copy(zeroEuler);
				REQUIRE(Serialize(context, zeroEuler));
				zeroEuler = Nz::EulerAnglesf(10, 24, 6); // Random values
				REQUIRE(zeroEuler != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &zeroEuler));
				REQUIRE(zeroEuler == copy);
			}

			THEN("Frustum")
			{
				context.stream->SetCursorPos(0);
				Nz::Frustumf frustum;
				frustum.Build(10, 10, 10, 100, Nz::Vector3f::UnitX(), Nz::Vector3f::UnitZ()); // Random values
				Nz::Frustumf copy(frustum);
				REQUIRE(Serialize(context, frustum));
				frustum.Build(50, 40, 20, 100, Nz::Vector3f::UnitX(), Nz::Vector3f::UnitZ());
				for (unsigned int i = 0; i <= Nz::BoxCorner_Max; ++i)
					REQUIRE(frustum.GetCorner(static_cast<Nz::BoxCorner>(i)) != copy.GetCorner(static_cast<Nz::BoxCorner>(i)));
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &frustum));
				for (unsigned int i = 0; i <= Nz::BoxCorner_Max; ++i)
					REQUIRE(frustum.GetCorner(static_cast<Nz::BoxCorner>(i)) == copy.GetCorner(static_cast<Nz::BoxCorner>(i)));
			}

			THEN("Matrix4")
			{
				context.stream->SetCursorPos(0);
				Nz::Matrix4f zeroMatrix = Nz::Matrix4f::Zero();
				Nz::Matrix4f copy(zeroMatrix);
				REQUIRE(Serialize(context, zeroMatrix));
				zeroMatrix = Nz::Matrix4f::Identity(); // Random values
				REQUIRE(zeroMatrix != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &zeroMatrix));
				REQUIRE(zeroMatrix == copy);
			}

			THEN("OrientedBox")
			{
				context.stream->SetCursorPos(0);
				Nz::OrientedBoxf zeroOBB = Nz::OrientedBoxf::Zero();
				Nz::OrientedBoxf copy(zeroOBB);
				REQUIRE(Serialize(context, zeroOBB));
				zeroOBB = Nz::OrientedBoxf(1, 1, 1, 1, 1, 1); // Random values
				REQUIRE(zeroOBB != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &zeroOBB));
				REQUIRE(zeroOBB == copy);
			}

			THEN("Plane")
			{
				context.stream->SetCursorPos(0);
				Nz::Planef planeXY = Nz::Planef::XY();
				Nz::Planef copy(planeXY);
				REQUIRE(Serialize(context, planeXY));
				planeXY = Nz::Planef::YZ();
				REQUIRE(planeXY != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &planeXY));
				REQUIRE(planeXY == copy);
			}

			THEN("Quaternion")
			{
				context.stream->SetCursorPos(0);
				Nz::Quaternionf quaternionIdentity = Nz::Quaternionf::Identity();
				Nz::Quaternionf copy(quaternionIdentity);
				REQUIRE(Serialize(context, quaternionIdentity));
				quaternionIdentity = Nz::Quaternionf::Zero();
				REQUIRE(quaternionIdentity != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &quaternionIdentity));
				REQUIRE(quaternionIdentity == copy);
			}

			THEN("Ray")
			{
				context.stream->SetCursorPos(0);
				Nz::Rayf axisX = Nz::Rayf::AxisX();
				Nz::Rayf copy(axisX);
				REQUIRE(Serialize(context, axisX));
				axisX = Nz::Rayf::AxisY();
				REQUIRE(axisX != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &axisX));
				REQUIRE(axisX == copy);
			}

			THEN("Rect")
			{
				context.stream->SetCursorPos(0);
				Nz::Rectf zeroRect = Nz::Rectf::Zero();
				Nz::Rectf copy(zeroRect);
				REQUIRE(Serialize(context, zeroRect));
				zeroRect = Nz::Rectf(1, 1, 1, 1); // Random values
				REQUIRE(zeroRect != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &zeroRect));
				REQUIRE(zeroRect == copy);
			}

			THEN("Sphere")
			{
				context.stream->SetCursorPos(0);
				Nz::Spheref zeroSphere = Nz::Spheref::Zero();
				Nz::Spheref copy(zeroSphere);
				REQUIRE(Serialize(context, zeroSphere));
				zeroSphere = Nz::Spheref::Unit();
				REQUIRE(zeroSphere != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &zeroSphere));
				REQUIRE(zeroSphere == copy);
			}

			THEN("Vector2")
			{
				context.stream->SetCursorPos(0);
				Nz::Vector2f unitX = Nz::Vector2f::UnitX();
				Nz::Vector2f copy(unitX);
				REQUIRE(Serialize(context, unitX));
				unitX = Nz::Vector2f::UnitY();
				REQUIRE(unitX != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &unitX));
				REQUIRE(unitX == copy);
			}

			THEN("Vector3")
			{
				context.stream->SetCursorPos(0);
				Nz::Vector3f unitX = Nz::Vector3f::UnitX();
				Nz::Vector3f copy(unitX);
				REQUIRE(Serialize(context, unitX));
				unitX = Nz::Vector3f::UnitY();
				REQUIRE(unitX != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &unitX));
				REQUIRE(unitX == copy);
			}

			THEN("Vector4")
			{
				context.stream->SetCursorPos(0);
				Nz::Vector4f unitX = Nz::Vector4f::UnitX();
				Nz::Vector4f copy(unitX);
				REQUIRE(Serialize(context, unitX));
				unitX = Nz::Vector4f::UnitY();
				REQUIRE(unitX != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &unitX));
				REQUIRE(unitX == copy);
			}
		}

		WHEN("We serialize core classes")
		{
			THEN("Color")
			{
				context.stream->SetCursorPos(0);
				Nz::Color red = Nz::Color::Red;
				Nz::Color copy(red);
				REQUIRE(Serialize(context, red));
				red = Nz::Color::Black;
				REQUIRE(red != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &red));
				REQUIRE(red == copy);
			}

			THEN("String")
			{
				context.stream->SetCursorPos(0);
				Nz::String string = "string";
				Nz::String copy(string);
				REQUIRE(Serialize(context, string));
				string = "another";
				REQUIRE(string != copy);
				context.stream->SetCursorPos(0);
				REQUIRE(Unserialize(context, &string));
				REQUIRE(string == copy);
			}
		}
	}
}
