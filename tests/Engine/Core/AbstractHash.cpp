#include <Nazara/Core/AbstractHash.hpp>
#include <Catch/catch.hpp>

#include <Nazara/Core/ByteArray.hpp>

#include <array>

SCENARIO("AbstractHash", "[CORE][ABSTRACTHASH]")
{
	GIVEN("The hash SHA512")
	{
		std::unique_ptr<Nz::AbstractHash> SHA512 = Nz::AbstractHash::Get(Nz::HashType_SHA512);
		SHA512->Begin();

		WHEN("We introduce data")
		{
			std::array<Nz::UInt8, 4> array{ { 0, 1, 2, 3 } };
			SHA512->Append(array.data(), array.size());

			THEN("We ask for the bytearray")
			{
				Nz::ByteArray byteArray = SHA512->End();
				CHECK(byteArray.GetSize() == SHA512->GetDigestLength());
			}
		}
	}
}
