#include <Nazara/Core/ByteStream.hpp>
#include <Catch/catch.hpp>

#include <array>

SCENARIO("ByteStream", "[CORE][BYTESTREAM]")
{
	GIVEN("A bytestream from a bunch of bytes")
	{
		const int numberOfBytes = 16;
		std::array<Nz::Int8, numberOfBytes> data;

		Nz::ByteStream byteStream(data.data(), numberOfBytes);

		WHEN("We write some data in it")
		{
			int value = 5;
			byteStream << value;
			Nz::String string = "string";
			byteStream << string;

			byteStream.FlushBits();

			THEN("We can retrieve them")
			{
				const void* const ptrData = data.data();
				Nz::ByteStream readStream;
				CHECK(readStream.GetSize() == 0);
				readStream = Nz::ByteStream(ptrData, byteStream.GetSize());
				int retrievedValue = 0;
				readStream >> retrievedValue;
				Nz::String retrievedString;
				readStream >> retrievedString;

				CHECK(value == retrievedValue);
				CHECK(string == retrievedString);
			}
		}
	}

	GIVEN("A bytestream with a byte array and a different endianness")
	{
		const int numberOfBytes = 16;
		Nz::ByteArray byteArray(numberOfBytes);
		Nz::ByteStream byteStream(&byteArray);

		byteStream.SetDataEndianness(Nz::GetPlatformEndianness() == Nz::Endianness_BigEndian ? Nz::Endianness_LittleEndian : Nz::Endianness_BigEndian);

		WHEN("We write an integer")
		{
			int value = 7;
			byteStream.Write(&value, sizeof(int));
			bool boolean = true;
			byteStream << boolean;
			byteStream.FlushBits();

			THEN("We can retrieve it properly")
			{
				Nz::ByteStream tmpStream(&byteArray);
				tmpStream.SetDataEndianness(byteStream.GetDataEndianness());

				int retrievedValue = 0;
				tmpStream.Read(&retrievedValue, sizeof(int));
				CHECK(value == retrievedValue);

				Nz::ByteStream readStream(std::move(tmpStream));
				bool retrievedBoolean = false;
				readStream >> retrievedBoolean;
				CHECK(boolean == retrievedBoolean);
			}
		}
	}
}