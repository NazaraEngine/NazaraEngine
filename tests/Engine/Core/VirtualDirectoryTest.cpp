#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <Nazara/Core/Hash/SHA256.hpp>
#include <catch2/catch.hpp>
#include <random>

std::filesystem::path GetResourceDir();

TEST_CASE("VirtualDirectory", "[Core][VirtualDirectory]")
{
	SECTION("Creating a virtual directory")
	{
		std::shared_ptr<Nz::VirtualDirectory> virtualDir = std::make_shared<Nz::VirtualDirectory>();
		WHEN("Iterating it, it only has . and ..")
		{
			bool dot = false;
			bool dotDot = false;
			virtualDir->Foreach([&](std::string_view name, const Nz::VirtualDirectory::Entry& /*entry*/)
			{
				if (name == ".")
				{
					CHECK_FALSE(dot);
					dot = true;
				}
				else if (name == "..")
				{
					CHECK_FALSE(dotDot);
					dotDot = true;
				}
				if (name != "." && name != "..")
					FAIL("Got file " << name);
			}, true);

			CHECK(dot);
			CHECK(dotDot);
		}
		AND_WHEN("Iterating it without dots, directory it empty")
		{
			virtualDir->Foreach([&](std::string_view name, const Nz::VirtualDirectory::Entry& /*entry*/)
			{
				FAIL("There should be nothing here, got " << name);
			});
		}
		AND_WHEN("We try to retrieve a non-existing file, it fails")
		{		
			CHECK_FALSE(virtualDir->GetEntry("File.bin", [](const Nz::VirtualDirectory::Entry& /*entry*/)
			{
				return true;
			}));
		
			CHECK_FALSE(virtualDir->GetEntry("Foo/File.bin", [](const Nz::VirtualDirectory::Entry& /*entry*/)
			{
				return true;
			}));
		
			CHECK_FALSE(virtualDir->GetEntry("Foo/Bar/File.bin", [](const Nz::VirtualDirectory::Entry& /*entry*/)
			{
				return true;
			}));

			virtualDir->StoreDirectory("Foo/Bar", std::make_shared<Nz::VirtualDirectory>());

			CHECK(virtualDir->GetEntry("Foo", [](const Nz::VirtualDirectory::Entry& entry)
			{
				return std::holds_alternative<Nz::VirtualDirectory::DirectoryEntry>(entry);
			}));
		
			CHECK(virtualDir->GetEntry("Foo/Bar", [](const Nz::VirtualDirectory::Entry& entry)
			{
				return std::holds_alternative<Nz::VirtualDirectory::DirectoryEntry>(entry);
			}));
		
			CHECK_FALSE(virtualDir->GetEntry("Foo/Bar/File.bin", [](const Nz::VirtualDirectory::Entry& /*entry*/)
			{
				return true;
			}));
		}

		std::mt19937 randGen(std::random_device{}());
		auto GenerateRandomData = [&]
		{
			std::vector<Nz::UInt8> randomData;
			for (std::size_t i = 0; i < 256; ++i)
			{
				unsigned int data = randGen();
				randomData.push_back(Nz::SafeCast<Nz::UInt8>((data & 0x000000FF) >> 0));
				randomData.push_back(Nz::SafeCast<Nz::UInt8>((data & 0x0000FF00) >> 8));
				randomData.push_back(Nz::SafeCast<Nz::UInt8>((data & 0x00FF0000) >> 16));
				randomData.push_back(Nz::SafeCast<Nz::UInt8>((data & 0xFF000000) >> 24));
			}

			return randomData;
		};

		auto CheckFile = [&](std::string_view path, const std::vector<Nz::UInt8>& expectedData)
		{
			return virtualDir->GetEntry(path, [&](const Nz::VirtualDirectory::Entry& entry)
			{
				if (!std::holds_alternative<Nz::VirtualDirectory::FileContentEntry>(entry))
				{
					FAIL("Target is not a file");
					return false;
				}

				const auto& contentEntry = std::get<Nz::VirtualDirectory::FileContentEntry>(entry);
				CHECK(std::equal(expectedData.begin(), expectedData.end(), contentEntry.data.begin(), contentEntry.data.end()));
				return true;
			});
		};

		WHEN("Storing a file")
		{
			auto randomData = GenerateRandomData();
			virtualDir->StoreFile("File.bin", randomData);

			WHEN("We retrieve it")
			{
				CHECK(CheckFile("File.bin", randomData));
			}
		}

		WHEN("Storing multiples files")
		{
			std::array paths = {
				"Abc",
				"Ab/cd",
				"po/mme\\de/terre.o",
				"Nazara",
				"Engine.exe",
				"Un/Deux/Trois",
				"Gnocchi.fromage",
				"Karmeliet.triple",
				"Mogwai.gremlins"
			};

			struct File
			{
				std::string path;
				std::vector<Nz::UInt8> data;
			};

			std::vector<File> files;
			std::unordered_map<std::string, std::size_t> filePathToIndex;

			for (const char* path : paths)
			{
				auto& file = files.emplace_back();
				file.data = GenerateRandomData();
				file.path = path;

				filePathToIndex[file.path] = files.size() - 1;
			}

			// Insert files into the virtual directory
			for (const File& file : files)
			{
				INFO("Storing " << file.path);
				CHECK_NOTHROW(virtualDir->StoreFile(file.path, file.data));
			}

			// Try to retrieve them
			for (const File& file : files)
			{
				INFO("Retrieving " << file.path);
				CHECK(CheckFile(file.path, file.data));
			}
		}
	}

	SECTION("Accessing filesystem using a VirtualDirectory")
	{
		std::shared_ptr<Nz::VirtualDirectory> virtualDir = std::make_shared<Nz::VirtualDirectory>(GetResourceDir());

		WHEN("Iterating, it's not empty")
		{
			bool empty = true;
			virtualDir->Foreach([&](std::string_view name, const Nz::VirtualDirectory::Entry& entry)
			{
				CHECK_FALSE(name == ".");
				CHECK_FALSE(name == "..");

				INFO("Only physical files and directories are expected");
				CHECK((std::holds_alternative<Nz::VirtualDirectory::PhysicalDirectoryEntry>(entry) || std::holds_alternative<Nz::VirtualDirectory::PhysicalFileEntry>(entry)));
				empty = false;
			});

			REQUIRE_FALSE(empty);
		}

		auto CheckFileHash = [&](const char* filepath, const char* expectedHash)
		{
			return virtualDir->GetEntry(filepath, [&](const Nz::VirtualDirectory::Entry& entry)
			{
				REQUIRE(std::holds_alternative<Nz::VirtualDirectory::PhysicalFileEntry>(entry));

				const auto& physFileEntry = std::get<Nz::VirtualDirectory::PhysicalFileEntry>(entry);

				Nz::File file(physFileEntry.filePath);

				Nz::SHA256Hash hash;
				WHEN("We compute " << hash.GetHashName() << " of " << physFileEntry.filePath << " file")
				{
					CHECK(Nz::ToUpper(Nz::ComputeHash(hash, file).ToHex()) == expectedHash);
				}
				return true;
			});
		};

		WHEN("Accessing files")
		{
			CHECK(CheckFileHash("Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash("./Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash("Engine/Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK(CheckFileHash("Engine/Audio/./The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash("The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash("Engine/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash("Engine/Audio/../The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));

			// We can't escape the virtual directory
			CHECK(CheckFileHash("../Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash("../../Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash("Engine/../Engine/Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK(CheckFileHash("../Engine/./Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash("../Engine/Engine/Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));

			auto CheckOurselves = [&](const auto& entry)
			{
				REQUIRE(std::holds_alternative<Nz::VirtualDirectory::DirectoryEntry>(entry));
				const auto& dirEntry = std::get<Nz::VirtualDirectory::DirectoryEntry>(entry);
				return dirEntry.directory == virtualDir;
			};

			CHECK(virtualDir->GetEntry("..", CheckOurselves));
			CHECK(virtualDir->GetEntry("../..", CheckOurselves));
			CHECK(virtualDir->GetEntry("./..", CheckOurselves));
			CHECK(virtualDir->GetEntry("./..", CheckOurselves));
			CHECK(virtualDir->GetEntry("Engine/../..", CheckOurselves));
			CHECK(virtualDir->GetEntry("Engine/../Engine/Audio/../../..", CheckOurselves));
		}
		AND_THEN("Overriding the physical file with another one")
		{
			virtualDir->StoreFile("Logo.png", GetResourceDir() / "ambience.ogg");
			CHECK(CheckFileHash("ambience.ogg", "49C486F44E43F023D54C9F375D902C21375DDB2748D3FA1863C9581D30E17F94"));
			CHECK(CheckFileHash("Logo.png", "49C486F44E43F023D54C9F375D902C21375DDB2748D3FA1863C9581D30E17F94"));
		}
	}
}
