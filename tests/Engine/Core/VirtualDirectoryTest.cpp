#include <Nazara/Core/VirtualDirectory.hpp>
#include <catch2/catch.hpp>
#include <random>

TEST_CASE("VirtualDirectory", "[Core][VirtualDirectory]")
{
	std::shared_ptr<Nz::VirtualDirectory> virtualDir = std::make_shared<Nz::VirtualDirectory>();

	WHEN("Iterating it, it only has . and ..")
	{
		bool failed = false;
		virtualDir->Foreach([&](const std::string& name, const Nz::VirtualDirectory::Entry& /*entry*/)
		{
			if (name != "." && name != "..")
				failed = true;
		}, true);

		CHECK_FALSE(failed);
	}
	AND_WHEN("Iterating it including dots, we get them")
	{
		bool failed = false;
		virtualDir->Foreach([&](const std::string& name, const Nz::VirtualDirectory::Entry& /*entry*/)
		{
			failed = true;
		});

		CHECK_FALSE(failed);
	}
	AND_WHEN("We try to retrieve a file, it fails")
	{
		Nz::VirtualDirectory::Entry entry;
		CHECK_FALSE(virtualDir->GetEntry("File.bin", &entry));
		CHECK_FALSE(virtualDir->GetEntry("Foo/File.bin", &entry));
		CHECK_FALSE(virtualDir->GetEntry("Foo/Bar/File.bin", &entry));

		virtualDir->StoreDirectory("Foo/Bar", std::make_shared<Nz::VirtualDirectory>());

		CHECK(virtualDir->GetEntry("Foo", &entry));
		CHECK(std::holds_alternative<Nz::VirtualDirectory::VirtualDirectoryEntry>(entry));
		CHECK(virtualDir->GetEntry("Foo/Bar", &entry));
		CHECK(std::holds_alternative<Nz::VirtualDirectory::VirtualDirectoryEntry>(entry));
		CHECK_FALSE(virtualDir->GetEntry("Foo/Bar/File.bin", &entry));
	}

	WHEN("Storing a file")
	{
		std::mt19937 randGen(std::random_device{}());
		std::vector<Nz::UInt8> randomData;
		for (std::size_t i = 0; i < 1024; ++i)
		{
			unsigned int data = randGen();
			randomData.push_back((data & 0x000000FF) >>  0);
			randomData.push_back((data & 0x0000FF00) >>  8);
			randomData.push_back((data & 0x00FF0000) >> 16);
			randomData.push_back((data & 0xFF000000) >> 24);
		}

		virtualDir->StoreFile("File.bin", randomData);

		WHEN("We retrieve it")
		{
			Nz::VirtualDirectory::Entry entry;
			REQUIRE(virtualDir->GetEntry("File.bin", &entry));

			REQUIRE(std::holds_alternative<Nz::VirtualDirectory::FileContentEntry>(entry));
			const auto& contentEntry = std::get<Nz::VirtualDirectory::FileContentEntry>(entry);
			CHECK(std::equal(randomData.begin(), randomData.end(), contentEntry.data->begin(), contentEntry.data->end()));
		}
	}
}
