#include <Nazara/Core/Uuid.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <regex>
#include <set>
#include <unordered_set>

SCENARIO("Uuid", "[CORE][UUID]")
{
	WHEN("Parsing UUID")
	{
		CHECK(Nz::Uuid::FromString("00000000-0000-0000-0000-000000000000") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-43e0-ba4c-e9334183b1f1") == Nz::Uuid({ 0x1B, 0x0E, 0x29, 0xAF, 0xFD, 0x4A, 0x43, 0xE0, 0xBA, 0x4C, 0xE9, 0x33, 0x41, 0x83, 0xB1, 0xF1 }));

		// Testing some invalid cases
		CHECK(Nz::Uuid::FromString("Nazara Engine") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af_fd4a_43e0_ba4c_e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a_43e0_ba4c_e93341-3b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("Zb0e29af-fd4a-43e0-ba4c-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29a\t-fd4a-43e0-ba4c-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4\v-43e0-ba4c-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-\r3e0-ba4c-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-43e\n-ba4c-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-43e0-\0a4c-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-43e0-ba4\n-e9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-43e0-ba4c-g9334183b1f1") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-fd4a-43e0-ba4c-e9334183b1fG") == Nz::Uuid());
		CHECK(Nz::Uuid::FromString("1b0e29af-HELL-OWOR-LDDD-e9334183b1f1") == Nz::Uuid());
	}

	WHEN("Generating a null UUID")
	{
		Nz::Uuid nullUuid;
		CHECK(nullUuid.IsNull());
		CHECK(nullUuid.ToArray() == std::array<Nz::UInt8, 16>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
		CHECK(nullUuid.ToString() == "00000000-0000-0000-0000-000000000000");
		CHECK(nullUuid.ToStringArray() == std::array<char, 37>{"00000000-0000-0000-0000-000000000000"});
		CHECK(nullUuid == Nz::Uuid{});
		CHECK(nullUuid >= Nz::Uuid{});
		CHECK(nullUuid <= Nz::Uuid{});
		CHECK_FALSE(nullUuid > Nz::Uuid{});
		CHECK_FALSE(nullUuid < Nz::Uuid{});
		CHECK(nullUuid != Nz::Uuid::Generate());
		CHECK(Nz::Uuid::FromString(nullUuid.ToString()) == nullUuid);
	}

	WHEN("Generating a UUID")
	{
		// https://stackoverflow.com/questions/136505/searching-for-uuids-in-text-with-regex
		std::regex uuidRegex(R"(^\b[0-9a-f]{8}\b-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-\b[0-9a-f]{12}\b$)");

		Nz::Uuid uuid = Nz::Uuid::Generate();
		INFO(uuid);
		CHECK_FALSE(uuid.IsNull());
		CHECK(std::regex_match(uuid.ToString(), uuidRegex));
		CHECK(uuid == uuid);
		CHECK(uuid == Nz::Uuid{uuid.ToArray()});
		CHECK(uuid >= uuid);
		CHECK(uuid <= uuid);
		CHECK_FALSE(uuid > uuid);
		CHECK_FALSE(uuid < uuid);
		CHECK(Nz::Uuid::FromString(uuid.ToString()) == uuid);
	}

	WHEN("Generating multiple UUID, they are unique")
	{
		std::set<Nz::Uuid> uuidSet;
		std::unordered_set<Nz::Uuid> uuidUnorderedset;

		auto InsertUniqueUuid = [](auto& container, const Nz::Uuid& uuid)
		{
			auto it = container.find(uuid);
			REQUIRE(it == container.end());
			container.insert(uuid);
		};

		for (std::size_t i = 0; i < 1'000; ++i)
		{
			auto uuid = Nz::Uuid::Generate();

			InsertUniqueUuid(uuidSet, uuid);
			InsertUniqueUuid(uuidUnorderedset, uuid);
		}
	}
}
