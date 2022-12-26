#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <array>
#include <filesystem>
#include <variant>

std::filesystem::path GetAssetDir();

TEST_CASE("ComputeHash", "[CORE][ALGORITHM]")
{
	std::filesystem::path testFilePath = GetAssetDir() / "Logo.png";

	struct Test
	{
		Nz::HashType hashType;
		std::variant<const char*, std::filesystem::path> input;
		const char* expectedOutput;
	};

	static_assert(Nz::CRC32("Nazara Engine") == 0x8A2F5235);
	static_assert(Nz::CRC32("The quick brown fox jumps over the lazy dog") == 0x414FA339);

	CHECK(Nz::CRC32("Nazara Engine") == 0x8A2F5235);
	CHECK(Nz::CRC32("The quick brown fox jumps over the lazy dog") == 0x414FA339);

	// https://defuse.ca/checksums.htm
	// https://toolslick.com/programming/hashing/crc-calculator

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

		//Test{ Nz::HashType::CRC16,      testFilePath, "30A6" },
		Test{ Nz::HashType::CRC32,      testFilePath, "5A2024CD" },
		Test{ Nz::HashType::CRC64,      testFilePath, "F92157F70C713EEC" },
		Test{ Nz::HashType::Fletcher16, testFilePath, "9152" },
		Test{ Nz::HashType::MD5,        testFilePath, "75B35EDB2DB8B4ED5020821E401B1FA3" },
		Test{ Nz::HashType::SHA1,       testFilePath, "FD6B51F6E176AA91BDDCFAFB6D65AF97116C3981" },
		Test{ Nz::HashType::SHA224,     testFilePath, "77F8536DB2AEDDB0B18747EF7907AEFFE019C3EB6C9E64CC31D2E4DA" },
		Test{ Nz::HashType::SHA256,     testFilePath, "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423" },
		Test{ Nz::HashType::SHA384,     testFilePath, "BB11F442F14AED77C116C6CA05103C006D40F861D23D272263BBD9F0D260FA5449B728A94F80BB807BD16778C558EF05" },
		Test{ Nz::HashType::SHA512,     testFilePath, "1EA50E73C1D7D8DFCD51AC2718D7EB953E4B2D1D9EFA06F5B89DC1B0315C6C57A007D733DFA4DA41BFCE0E73446EFAF3413E8C1D38A0C327773AFF49C010F091" },
		Test{ Nz::HashType::Whirlpool,  testFilePath, "A1598870C7E5C59004CD0A2C7E17248606E2DDD832EED1B0E2D52A9A72842A073CC2F889D2EC71C061A21A86879FF009D4FED1010B454FF8535C401BC9A60F64" },
	};

	for (const Test& test : tests)
	{
		auto hash = Nz::AbstractHash::Get(test.hashType);

		if (std::holds_alternative<const char*>(test.input))
		{
			const char* inputStr = std::get<const char*>(test.input);
			WHEN("We compute " << hash->GetHashName() << " of '" << inputStr << "'")
			{
				auto result = Nz::ComputeHash(*hash, inputStr);
				CHECK(result.GetSize() == hash->GetDigestLength());
				CHECK(Nz::ToUpper(result.ToHex()) == test.expectedOutput);
			}
		}
		else
		{
			assert(std::holds_alternative<std::filesystem::path>(test.input));
			const std::filesystem::path& filePath = std::get<std::filesystem::path>(test.input);
			Nz::File file(filePath);

			WHEN("We compute " << hash->GetHashName() << " of " << filePath << " file")
			{
				auto result = Nz::ComputeHash(*hash, file);
				CHECK(result.GetSize() == hash->GetDigestLength());
				CHECK(Nz::ToUpper(result.ToHex()) == test.expectedOutput);
			}
		}
	}
}
