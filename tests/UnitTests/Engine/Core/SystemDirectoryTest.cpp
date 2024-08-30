#include <Nazara/Core/SystemDirectory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

SCENARIO("SystemDirectory", "[CORE][SystemDirectory]")
{
	CHECK(Nz::GetApplicationDirectory(Nz::ApplicationDirectory::Cache, "Nazara"));
	CHECK(Nz::GetApplicationDirectory(Nz::ApplicationDirectory::Config, "Nazara"));
	CHECK(Nz::GetApplicationDirectory(Nz::ApplicationDirectory::Data, "Nazara"));
	CHECK(Nz::GetApplicationDirectory(Nz::ApplicationDirectory::SavedGames, "Nazara"));

	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::Cache));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::Fonts));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::Temporary));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserConfig));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserDesktop));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserDocuments));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserDownloads));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserHome));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserMusic));
	CHECK(Nz::GetSystemDirectory(Nz::SystemDirectory::UserPictures));
}
