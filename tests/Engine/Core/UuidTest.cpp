#include <Nazara/Core/Uuid.hpp>
#include <catch2/catch.hpp>
#include <regex>
#include <set>
#include <unordered_set>

SCENARIO("Uuid", "[CORE][UUID]")
{
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
