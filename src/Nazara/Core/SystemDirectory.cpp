// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/SystemDirectory.hpp>
#include <Nazara/Core/EnvironmentVariables.hpp>
#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/PathUtils.hpp>
#include <array>
#include <optional>

#ifdef NAZARA_PLATFORM_WINDOWS
#include <Windows.h>
#include <fileapi.h>
#include <Shlobj.h>
#include <userenv.h>
#endif

#undef GetEnvironmentVariable
#undef GetSystemDirectory

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
	{
		struct Env
		{
			const char* name;
		};

		Result<std::filesystem::path, std::string> Absolute(const std::filesystem::path& path)
		{
			std::error_code ec;
			std::filesystem::path absolutePath = std::filesystem::absolute(path, ec);
			if (ec)
				return Err(ec.message());

			return absolutePath;
		}

		Result<std::filesystem::path, std::string> Absolute(std::string_view path)
		{
			return Absolute(Utf8Path(path));
		}

		Result<std::filesystem::path, std::string> ToPath(Env env)
		{
			const char* value = GetEnvironmentVariable(env.name);
			if (!value || *value == '\0')
				return Err(Format("no env {}", env.name)); //< keep it short to avoid allocation

			return Utf8Path(value);
		}

		Result<std::reference_wrapper<const std::filesystem::path>, std::string> ToPath(const std::filesystem::path& p)
		{
			return Ok(p);
		}

		Result<std::filesystem::path, std::string> ToPath(const char* str)
		{
			return Utf8Path(str);
		}

		Result<std::filesystem::path, std::string> ToPath(std::string_view str)
		{
			return Utf8Path(str);
		}

		template<typename P1, typename P2, typename... Args>
		Result<std::filesystem::path, std::string> JoinPathImpl(P1&& part1, P2&& part2, Args&&... args)
		{
			if (!part1)
				return part1;

			if (!part2)
				return part2;

			if constexpr (sizeof...(args) > 0)
				return JoinPathImpl(ToPath(*part1 / *part2), std::forward<Args>(args)...);
			else
				return *part1 / *part2;
		}

#ifdef NAZARA_PLATFORM_WINDOWS
		static_assert(std::is_same_v<std::filesystem::path::value_type, wchar_t>);

		Result<std::filesystem::path, std::string> GetHomeDirectory()
		{
			HANDLE processHandle = ::GetCurrentProcess();
			HANDLE processToken;
			if NAZARA_UNLIKELY(!::OpenProcessToken(processHandle, TOKEN_QUERY, &processToken))
				return Err(Error::GetLastSystemError());

			NAZARA_DEFER(::CloseHandle(processToken););

			// get path length first
			DWORD bufferSize = 0;

			::GetUserProfileDirectoryW(processToken, nullptr, &bufferSize);
			if (bufferSize == 0)
				return Err(Error::GetLastSystemError());

			// and then get path
			std::wstring path(bufferSize - 1, L'\0');
			if NAZARA_UNLIKELY(!::GetUserProfileDirectoryW(processToken, &path[0], &bufferSize))
				return Err(Error::GetLastSystemError());

			return std::filesystem::path(std::move(path));
		}

		Result<std::filesystem::path, std::string> GetTempDirectory()
		{
			// According to https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppathw
			std::array<wchar_t, MAX_PATH + 1> tempPath;
			DWORD pathLength = ::GetTempPathW(SafeCaster(tempPath.size()), &tempPath[0]);
			if (pathLength == 0)
				return Err(Error::GetLastSystemError());

			return std::filesystem::path(tempPath.begin(), tempPath.begin() + pathLength - 1);
		}

		Result<std::filesystem::path, std::string> ToPath(REFKNOWNFOLDERID rfid)
		{
			LPWSTR path;
			if (HRESULT result = ::SHGetKnownFolderPath(rfid, KF_FLAG_DONT_VERIFY, nullptr, &path); FAILED(result))
				return Err(Error::GetLastSystemError(result));

			NAZARA_DEFER(::CoTaskMemFree(path););

			return Ok(std::filesystem::path(path));
		}
#endif

		template<typename... Args>
		Result<std::filesystem::path, std::string> JoinPath(Args&&... args)
		{
			return JoinPathImpl(ToPath(args)...);
		}
	}

	Result<std::filesystem::path, std::string> GetApplicationDirectory(ApplicationDirectory applicationDirectory, std::string_view applicationName)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

#if defined(NAZARA_PLATFORM_WINDOWS)
		switch (applicationDirectory)
		{
			case ApplicationDirectory::Cache:      return JoinPath(FOLDERID_LocalAppData, applicationName, "Cache");
			case ApplicationDirectory::Config:     return JoinPath(FOLDERID_LocalAppData, applicationName, "Config");
			case ApplicationDirectory::Data:       return JoinPath(FOLDERID_LocalAppData, applicationName, "Data");
			case ApplicationDirectory::SavedGames: return JoinPath(FOLDERID_SavedGames, applicationName);
		}
#elif defined(NAZARA_PLATFORM_LINUX)
		switch (applicationDirectory)
		{
			case ApplicationDirectory::Cache:
			{
				auto dir = JoinPath(Env{ "HOME" }, ".cache", applicationName);
				if (!dir)
					dir = JoinPath(".cache", applicationName);

				return dir;
			}

			case ApplicationDirectory::Config:
			{
				auto dir = JoinPath(Env{ "XDG_CONFIG_HOME" }, applicationName);
				if (!dir)
					dir = JoinPath(Env{ "HOME" }, ".config", applicationName);
				if (!dir)
					dir = JoinPath(".config", applicationName);

				return dir;
			}

			case ApplicationDirectory::Data:
			{
				auto dir = JoinPath(Env{ "XDG_DATA_HOME" }, applicationName, "data");
				if (!dir)
					dir = JoinPath(Env{ "HOME" }, ".local/share", applicationName, "data");
				if (!dir)
					dir = JoinPath(".local/share", applicationName, "data");

				return dir;
			}

			case ApplicationDirectory::SavedGames:
			{
				auto dir = JoinPath(Env{ "XDG_DATA_HOME" }, applicationName, "saves");
				if (!dir)
					dir = JoinPath(Env{ "HOME" }, ".local/share", applicationName, "save");
				if (!dir)
					dir = JoinPath(".local/share", applicationName, "saves");

				return dir;
			}
		}
#elif defined(NAZARA_PLATFORM_MACOS)
		switch (applicationDirectory)
		{
			// TODO: Use NSSearchPathForDirectoriesInDomains
			case ApplicationDirectory::Cache:      return JoinPath("~/Library/Caches", applicationName);
			case ApplicationDirectory::Config:     return JoinPath("~/Library/Preferences", applicationName);
			case ApplicationDirectory::Data:       return JoinPath("~/Library/Application Support", applicationName, "Data");
			case ApplicationDirectory::SavedGames: return JoinPath("~/Library/Application Support", applicationName, "Saves");
		}
#elif defined(NAZARA_PLATFORM_WEB)
		switch (applicationDirectory)
		{
			// FIXME: Probably not correct
			case ApplicationDirectory::Cache:      return JoinPath(applicationName, "cache");
			case ApplicationDirectory::Config:     return JoinPath(applicationName, "persistent/config");
			case ApplicationDirectory::Data:       return JoinPath(applicationName, "persistent/data");
			case ApplicationDirectory::SavedGames: return JoinPath(applicationName, "persistent/saves");
		}
#else
	#error Missing implementation on this platform
#endif

		return Err("unhandled");
	}

	Result<std::filesystem::path, std::string> GetSystemDirectory(SystemDirectory systemDirectory)
	{
		NAZARA_USE_ANONYMOUS_NAMESPACE

#if defined(NAZARA_PLATFORM_WINDOWS)
		switch (systemDirectory)
		{
			case SystemDirectory::Temporary:
			{
				auto dir = GetTempDirectory();
				if (!dir)
					dir = ToPath(Env{ "TEMP" });

				return dir;
			}

			case SystemDirectory::UserHome:
			{
				auto dir = GetHomeDirectory();
				if (!dir)
				{
					dir = ToPath(Env{ "USERPROFILE" });
					if (!dir)
					{
						dir = JoinPath(Env{ "HOMEDRIVE" }, Env{ "HOMEPATH" });
						if (!dir)
							dir = ToPath(Env{ "HOME" });
					}
				}

				return GetHomeDirectory();
			}

			case SystemDirectory::Cache:         return JoinPath(FOLDERID_LocalAppData, "Cache");
			case SystemDirectory::Fonts:         return ToPath(FOLDERID_Fonts);
			case SystemDirectory::UserConfig:    return ToPath(FOLDERID_LocalAppData);
			case SystemDirectory::UserDesktop:   return ToPath(FOLDERID_Desktop);
			case SystemDirectory::UserDownloads: return ToPath(FOLDERID_Downloads);
			case SystemDirectory::UserDocuments: return ToPath(FOLDERID_Documents);
			case SystemDirectory::UserMusic:     return ToPath(FOLDERID_Music);
			case SystemDirectory::UserPictures:  return ToPath(FOLDERID_Pictures);
			case SystemDirectory::UserVideos:    return ToPath(FOLDERID_Videos);
		}
#elif defined(NAZARA_PLATFORM_LINUX)
		const char* linuxHomeEnvironment = GetEnvironmentVariable(env.name);
		bool hasHomeEnv = (!linuxHomeEnvironment || *linuxHomeEnvironment == '\0');

		switch (systemDirectory)
		{
			case SystemDirectory::Cache:
			{
				auto dir = ToPath(Env{ "XDG_CACHE_HOME" });
				if (!dir)
					dir = JoinPath(Env{ "HOME" }, ".cache");
				if (!dir)
					dir = ToPath(".cache");

				return dir;
			}

			case SystemDirectory::Temporary:
			{
				auto dir = ToPath(Env{ "TMPDIR" });
				if (!dir)
				{
					dir = ToPath(Env{ "TEMP" });
					if (!dir)
					{
						dir = ToPath(Env{ "TMP" });
						if (!dir)
							dir = ToPath("/tmp");
					}
				}

				return dir;
			}

			case SystemDirectory::UserConfig:
			{
				auto dir = ToPath(Env{ "XDG_CONFIG_HOME" });
				if (!dir)
					dir = JoinPath(Env{ "HOME" }, ".config");
				if (!dir)
					dir = ToPath(".config");

				return dir;
			}

			case SystemDirectory::Fonts:         return (hasHomeEnv ? JoinPath(Env{ "HOME" }, ".fonts")    : ToPath(".fonts"));
			case SystemDirectory::UserDesktop:   return (hasHomeEnv ? JoinPath(Env{ "HOME" }, "Desktop")   : ToPath("Desktop"));
			case SystemDirectory::UserDownloads: return (hasHomeEnv ? JoinPath(Env{ "HOME" }, "Downloads") : ToPath("Downloads"));
			case SystemDirectory::UserDocuments: return (hasHomeEnv ? JoinPath(Env{ "HOME" }, "Documents") : ToPath("Documents"));
			case SystemDirectory::UserHome:      return (hasHomeEnv ? ToPath(Env{ "HOME" })                : ToPath("./"));
			case SystemDirectory::UserMusic:     return (hasHomeEnv ? JoinPath(Env{ "HOME" }, "Music")     : ToPath("Music"));
			case SystemDirectory::UserPictures:  return (hasHomeEnv ? JoinPath(Env{ "HOME" }, "Pictures")  : ToPath("Pictures"));
			case SystemDirectory::UserVideos:    return (hasHomeEnv ? JoinPath(Env{ "HOME" }, "Videos")    : ToPath("Videos"));
		}
#elif defined(NAZARA_PLATFORM_MACOS)
		switch (systemDirectory)
		{
			// TODO: Use NSSearchPathForDirectoriesInDomains
			case SystemDirectory::Cache:     return ToPath("~/Library/Caches");
			case SystemDirectory::Fonts:     return ToPath("/System/Library/Fonts");
			case SystemDirectory::Temporary:
			{
				auto dir = ToPath(Env{ "TMPDIR" });
				if (!dir)
					dir = ToPath("/tmp");

				return dir;
			}

			case SystemDirectory::UserConfig:    return ToPath("~/Library/Preferences");
			case SystemDirectory::UserDesktop:   return ToPath("~/Desktop");
			case SystemDirectory::UserDownloads: return ToPath("~/Downloads");
			case SystemDirectory::UserDocuments: return ToPath("~/Documents");
			case SystemDirectory::UserHome:      return ToPath("~");
			case SystemDirectory::UserMusic:     return ToPath("~/Music");
			case SystemDirectory::UserPictures:  return ToPath("~/Pictures");
			case SystemDirectory::UserVideos:    return ToPath("~/Videos");
		}
#elif defined(NAZARA_PLATFORM_WEB)
		return Err("unsupported");
#else
	#error Missing implementation on this platform
#endif

		return Err("unhandled");
	}
}

#ifdef NAZARA_PLATFORM_WINDOWS
#include <Nazara/Core/AntiWindows.hpp>
#endif
