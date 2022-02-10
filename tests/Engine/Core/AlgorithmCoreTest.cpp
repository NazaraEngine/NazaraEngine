#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <catch2/catch.hpp>
#include <array>

TEST_CASE("Apply", "[CORE][ALGORITHM]")
{
	SECTION("Apply lambda to two vector2")
	{
		Nz::Vector2<int> vector = Nz::Vector2<int>::Unit();
		auto lambda = [](const Nz::Vector2<int>& vec1, const Nz::Vector2<int>& vec2)
		{
			return vec1 + vec2;
		};

		Nz::Vector2<int> result = Nz::Apply(lambda, std::make_tuple(vector, vector));

		REQUIRE(result == (Nz::Vector2<int>::Unit() * 2));
	}

	/*SECTION("Apply member function to vector2")
	{
		Nz::Vector2<int> vector = Nz::Vector2<int>::Unit();

		int result = Nz::Apply(vector, (int(Nz::Vector2<int>::*)(const Nz::Vector2<int>&)) &Nz::Vector2<int>::Distance<int>, std::make_tuple(vector));

		REQUIRE(result == 0);
	}*/
}

TEST_CASE("ComputeHash", "[CORE][ALGORITHM]")
{
	struct Test
	{
		Nz::HashType hashType;
		const char* input;
		const char* expectedOutput;
	};

	std::array tests{
		//Test{ Nz::HashType::CRC16,      "Nazara Engine", "9412" },
		Test{ Nz::HashType::CRC32,      "Nazara Engine", "8A2F5235" },
		Test{ Nz::HashType::CRC64,      "Nazara Engine", "87211217C5FFCDDD" },
		Test{ Nz::HashType::Fletcher16, "Nazara Engine", "71D7" },
		Test{ Nz::HashType::MD5,        "Nazara Engine", "71FF4EC3B56010ABC03E4B2C1C8A14B9" },
		Test{ Nz::HashType::SHA1,       "Nazara Engine", "FCE7C077A1ED0881A8C60C4822BB1369B672CA5B" },
		Test{ Nz::HashType::SHA224,     "Nazara Engine", "673677224E9B0D24C3828FE6CD36A37C2D43BA27C31B6E6E54756BD4" },
		Test{ Nz::HashType::SHA256,     "Nazara Engine", "7E66722931B65BF780FB55D41834C3D67455029CC1CB497976D2D41A90D3FD4C" },
		Test{ Nz::HashType::SHA384,     "Nazara Engine", "80064D11A4E4C2A44DE03406E03025C52641E04BA80DE78B1BB0BA6EA577B4B6914F2BDED5B95BB7285F8EA785B9B996" },
		Test{ Nz::HashType::SHA512,     "Nazara Engine", "C3A8212B61B88D77E8C4B40884D49BA6A54202865CAA847F676D2EA20E60F43B1C8024DE982A214EB3670B752AF3EE37189F1EBDCA608DD0DD427D8C19371FA5" },
		Test{ Nz::HashType::Whirlpool,  "Nazara Engine", "92113DC95C25057C4154E9A8B2A4C4C800D24DD22FA7D796F300AF9C4EFA4FAAB6030F66B0DC74B270A911DA18E007544B79B84440A1D58AA7C79A73C39C29F8" },

		//Test{ Nz::HashType::CRC16,      "The quick brown fox jumps over the lazy dog", "FCDF" },
		Test{ Nz::HashType::CRC32,      "The quick brown fox jumps over the lazy dog", "414FA339" },
		Test{ Nz::HashType::CRC64,      "The quick brown fox jumps over the lazy dog", "41E05242FFA9883B" },
		Test{ Nz::HashType::Fletcher16, "The quick brown fox jumps over the lazy dog", "FEE8" },
		Test{ Nz::HashType::MD5,        "The quick brown fox jumps over the lazy dog", "9E107D9D372BB6826BD81D3542A419D6" },
		Test{ Nz::HashType::SHA1,       "The quick brown fox jumps over the lazy dog", "2FD4E1C67A2D28FCED849EE1BB76E7391B93EB12" },
		Test{ Nz::HashType::SHA224,     "The quick brown fox jumps over the lazy dog", "730E109BD7A8A32B1CB9D9A09AA2325D2430587DDBC0C38BAD911525" },
		Test{ Nz::HashType::SHA256,     "The quick brown fox jumps over the lazy dog", "D7A8FBB307D7809469CA9ABCB0082E4F8D5651E46D3CDB762D02D0BF37C9E592" },
		Test{ Nz::HashType::SHA384,     "The quick brown fox jumps over the lazy dog", "CA737F1014A48F4C0B6DD43CB177B0AFD9E5169367544C494011E3317DBF9A509CB1E5DC1E85A941BBEE3D7F2AFBC9B1" },
		Test{ Nz::HashType::SHA512,     "The quick brown fox jumps over the lazy dog", "07E547D9586F6A73F73FBAC0435ED76951218FB7D0C8D788A309D785436BBB642E93A252A954F23912547D1E8A3B5ED6E1BFD7097821233FA0538F3DB854FEE6" },
		Test{ Nz::HashType::Whirlpool,  "The quick brown fox jumps over the lazy dog", "B97DE512E91E3828B40D2B0FDCE9CEB3C4A71F9BEA8D88E75C4FA854DF36725FD2B52EB6544EDCACD6F8BEDDFEA403CB55AE31F03AD62A5EF54E42EE82C3FB35" },
	};

	for (const Test& test : tests)
	{
		auto hash = Nz::AbstractHash::Get(test.hashType);

		WHEN("We compute " << hash->GetHashName() << " of '" << test.input << '"')
		{
			auto result = Nz::ComputeHash(*hash, test.input);
			REQUIRE(Nz::ToUpper(result.ToHex()) == test.expectedOutput);
		}
	}
}
