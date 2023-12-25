#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Core/VirtualDirectory.hpp>
#include <Nazara/Core/VirtualDirectoryFilesystemResolver.hpp>
#include <Nazara/Core/Hash/SHA256.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <random>

std::filesystem::path GetAssetDir();

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
			Nz::ByteArray randomData;
			for (std::size_t i = 0; i < 256; ++i)
			{
				unsigned int data = randGen();
				randomData.PushBack(Nz::SafeCast<Nz::UInt8>((data & 0x000000FF) >> 0));
				randomData.PushBack(Nz::SafeCast<Nz::UInt8>((data & 0x0000FF00) >> 8));
				randomData.PushBack(Nz::SafeCast<Nz::UInt8>((data & 0x00FF0000) >> 16));
				randomData.PushBack(Nz::SafeCast<Nz::UInt8>((data & 0xFF000000) >> 24));
			}

			return randomData;
		};

		auto CheckFile = [&](std::string_view path, const Nz::ByteArray& expectedData)
		{
			return virtualDir->GetFileContent(path, [&](const void* data, std::size_t length)
			{
				return length == expectedData.size() && std::memcmp(data, &expectedData[0], length) == 0;
			});
		};

		auto CheckFileContent = [&](std::string_view path, const Nz::ByteArray& expectedData)
		{
			return virtualDir->GetFileContent(path, [&](const void* data, std::size_t size)
			{
				if (expectedData.size() != size)
				{
					FAIL("size doesn't match");
					return false;
				}

				return std::memcmp(&expectedData[0], data, expectedData.size()) == 0;
			});
		};

		WHEN("Storing a file")
		{
			auto randomData = GenerateRandomData();
			virtualDir->StoreFile("File.bin", randomData);

			WHEN("We retrieve it")
			{
				CHECK(CheckFile("File.bin", randomData));
				CHECK(CheckFileContent("File.bin", randomData));
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
				Nz::ByteArray data;
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
				INFO("Retrieving " << file.path << " using GetFileContent");
				CHECK(CheckFileContent(file.path, file.data));
			}
		}
	}

	SECTION("Accessing filesystem using a VirtualDirectory")
	{
		std::shared_ptr<Nz::VirtualDirectory> resourceDir = std::make_shared<Nz::VirtualDirectory>(std::make_shared<Nz::VirtualDirectoryFilesystemResolver>(GetAssetDir()));

		WHEN("Iterating, it's not empty")
		{
			bool empty = true;
			resourceDir->Foreach([&](std::string_view name, const Nz::VirtualDirectory::Entry& entry)
			{
				CHECK_FALSE(name == ".");
				CHECK_FALSE(name == "..");

				INFO("Only physical files and directories are expected");
				CHECK((std::holds_alternative<Nz::VirtualDirectory::DirectoryEntry>(entry) || std::holds_alternative<Nz::VirtualDirectory::FileEntry>(entry)));
				empty = false;
			});

			REQUIRE_FALSE(empty);
		}

		auto CheckFileHash = [](const Nz::VirtualDirectoryPtr& dir, const char* filepath, const char* expectedHash)
		{
			return dir->GetEntry(filepath, [&](const Nz::VirtualDirectory::Entry& entry)
			{
				REQUIRE(std::holds_alternative<Nz::VirtualDirectory::FileEntry>(entry));

				const auto& physFileEntry = std::get<Nz::VirtualDirectory::FileEntry>(entry);

				Nz::SHA256Hasher hash;
				WHEN("We compute " << hash.GetHashName() << " of " << physFileEntry.stream->GetPath() << " file")
				{
					CHECK(Nz::ToUpper(Nz::ComputeHash(hash, *physFileEntry.stream).ToHex()) == expectedHash);
				}
				return true;
			});
		};

		auto CheckFileContentHash = [](const Nz::VirtualDirectoryPtr& dir, const char* filepath, const char* expectedHash)
		{
			return dir->GetFileContent(filepath, [&](const void* data, std::size_t size)
			{
				Nz::SHA256Hasher hash;
				WHEN("We compute " << hash.GetHashName() << " of " << filepath << " file")
				{
					hash.Begin();
					hash.Append(static_cast<const Nz::UInt8*>(data), size);
					CHECK(Nz::ToUpper(hash.End().ToHex()) == expectedHash);
				}
			});
		};

		WHEN("Accessing files")
		{
			CHECK(CheckFileHash(resourceDir, "Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash(resourceDir, "./Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash(resourceDir, "Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK(CheckFileHash(resourceDir, "Audio/./The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash(resourceDir, "The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash(resourceDir, "The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash(resourceDir, "Audio/../The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));

			// We can't escape the virtual directory
			CHECK(CheckFileHash(resourceDir, "../Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash(resourceDir, "../../Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash(resourceDir, "../../Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK(CheckFileHash(resourceDir, ".././Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK_FALSE(CheckFileHash(resourceDir, "../Tests/Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));

			auto CheckOurselves = [&](const auto& entry)
			{
				REQUIRE(std::holds_alternative<Nz::VirtualDirectory::DirectoryEntry>(entry));
				const auto& dirEntry = std::get<Nz::VirtualDirectory::DirectoryEntry>(entry);
				return dirEntry.directory == resourceDir;
			};

			CHECK(resourceDir->GetEntry("..", CheckOurselves));
			CHECK(resourceDir->GetEntry("../..", CheckOurselves));
			CHECK(resourceDir->GetEntry("./..", CheckOurselves));
			CHECK(resourceDir->GetEntry("./..", CheckOurselves));
			CHECK(resourceDir->GetEntry("Audio/../..", CheckOurselves));
			CHECK(resourceDir->GetEntry("Core/../Audio/../../..", CheckOurselves));
		}
		AND_THEN("Overriding the physical file with another one")
		{
			resourceDir->StoreFile("Logo.png", std::make_shared<Nz::File>(GetAssetDir() / "Audio/ambience.ogg", Nz::OpenMode::Read));

			CHECK(CheckFileHash(resourceDir, "Audio/ambience.ogg", "49C486F44E43F023D54C9F375D902C21375DDB2748D3FA1863C9581D30E17F94"));
			CHECK(CheckFileHash(resourceDir, "Logo.png", "49C486F44E43F023D54C9F375D902C21375DDB2748D3FA1863C9581D30E17F94"));
			CHECK(CheckFileContentHash(resourceDir, "Audio/ambience.ogg", "49C486F44E43F023D54C9F375D902C21375DDB2748D3FA1863C9581D30E17F94"));
			CHECK(CheckFileContentHash(resourceDir, "Logo.png", "49C486F44E43F023D54C9F375D902C21375DDB2748D3FA1863C9581D30E17F94"));
		}

		WHEN("Accessing physical folder as a virtual folder")
		{
			CHECK(resourceDir->GetDirectoryEntry("Utility", [&](const Nz::VirtualDirectory::DirectoryEntry& directoryEntry)
			{
				bool found = false;
				directoryEntry.directory->Foreach([&](std::string_view entryName, const Nz::VirtualDirectory::Entry& entry)
				{
					if (entryName == "GIF")
					{
						CHECK(std::holds_alternative<Nz::VirtualDirectory::DirectoryEntry>(entry));
						found = true;
					}
				});

				return found;
			}));
		}

		WHEN("Testing uproot escape")
		{
			std::shared_ptr<Nz::VirtualDirectory> engineDir = std::make_shared<Nz::VirtualDirectory>(std::make_shared<Nz::VirtualDirectoryFilesystemResolver>(GetAssetDir() / "Audio"));

			CHECK_FALSE(engineDir->IsUprootAllowed());

			// We can't escape the virtual directory
			CHECK_FALSE(engineDir->Exists("../Logo.png"));
			CHECK_FALSE(engineDir->Exists("../../Logo.png"));
			CHECK_FALSE(engineDir->Exists("../Tests/Audio/Audio/The_Brabanconne.ogg"));
			CHECK(CheckFileHash(engineDir, "../../The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
			CHECK(CheckFileHash(engineDir, ".././The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));

			engineDir->AllowUproot(true);
			CHECK(engineDir->IsUprootAllowed());

			// Now we're able to access the asset folder beneath
			CHECK(CheckFileHash(engineDir, "../Logo.png", "5C4B9387327C039A6CE9ED51983D6C2ADA9F9DD01D024C2D5D588237ADFC7423"));
			CHECK(CheckFileHash(engineDir, "../Audio/The_Brabanconne.ogg", "E07706E0BEEC7770CDE36008826743AF9EEE5C80CA0BD83C37771CBC8B52E738"));
		}
	}
}
