// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Shader module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Shader/FilesystemModuleResolver.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <efsw/efsw.h>
#include <cassert>
#include <Nazara/Shader/Debug.hpp>

namespace Nz
{
	FilesystemModuleResolver::FilesystemModuleResolver()
	{
		m_fileWatcher = efsw_create(0);
		efsw_watch(m_fileWatcher);
	}

	FilesystemModuleResolver::~FilesystemModuleResolver()
	{
		if (m_fileWatcher)
			efsw_release(m_fileWatcher);
	}

	void FilesystemModuleResolver::RegisterModule(const std::filesystem::path& realPath)
	{
		ShaderAst::ModulePtr module;
		try
		{
			std::string ext = realPath.extension().generic_u8string();
			if (ext == CompiledModuleExtension)
			{
				Nz::File file(realPath);
				if (!file.Open(Nz::OpenMode::ReadOnly | Nz::OpenMode::Text))
					throw std::runtime_error("failed to open " + realPath.generic_u8string());

				std::size_t length = static_cast<std::size_t>(file.GetSize());

				std::vector<Nz::UInt8> content(length);
				if (length > 0 && file.Read(&content[0], length) != length)
					throw std::runtime_error("failed to read " + realPath.generic_u8string());

				module = ShaderAst::UnserializeShader(content.data(), content.size());
			}
			else if (ext == ModuleExtension)
				module = ShaderLang::ParseFromFile(realPath);
			else
				throw std::runtime_error("unknown extension " + ext);
		}
		catch (const std::exception& e)
		{
			NazaraError("failed to register module from file " + realPath.generic_u8string() + ": " + e.what());
			return;
		}

		if (!module)
			return;

		std::string moduleName = module->metadata->moduleName;
		RegisterModule(std::move(module));

		std::filesystem::path canonicalPath = std::filesystem::canonical(realPath);
		m_moduleByFilepath.emplace(canonicalPath.generic_u8string(), std::move(moduleName));
	}

	void FilesystemModuleResolver::RegisterModule(std::string_view moduleSource)
	{
		ShaderAst::ModulePtr module = ShaderLang::Parse(moduleSource);
		if (!module)
			return;

		return RegisterModule(std::move(module));
	}

	void FilesystemModuleResolver::RegisterModule(ShaderAst::ModulePtr module)
	{
		assert(module);

		std::string moduleName = module->metadata->moduleName;
		if (moduleName.empty())
			throw std::runtime_error("cannot register anonymous module");

		auto it = m_modules.find(moduleName);
		if (it != m_modules.end())
		{
			it->second = std::move(module);

			OnModuleUpdated(this, moduleName);
		}
		else
			m_modules.emplace(std::move(moduleName), std::move(module));
	}

	void FilesystemModuleResolver::RegisterModuleDirectory(const std::filesystem::path& realPath, bool watchDirectory)
	{
		if (!std::filesystem::is_directory(realPath))
			return;

		auto FileSystemCallback = [](efsw_watcher /*watcher*/, efsw_watchid /*watchid*/, const char* dir, const char* filename, efsw_action action, const char* oldFileName, void* param)
		{
			FilesystemModuleResolver* resolver = static_cast<FilesystemModuleResolver*>(param);

			switch (action)
			{
				case EFSW_ADD:
					resolver->OnFileAdded(dir, filename);
					break;

				case EFSW_DELETE:
					resolver->OnFileRemoved(dir, filename);
					break;

				case EFSW_MODIFIED:
					resolver->OnFileUpdated(dir, filename);
					break;

				case EFSW_MOVED:
					resolver->OnFileMoved(dir, filename, (oldFileName) ? oldFileName : std::string_view());
					break;
			}
		};

		if (watchDirectory)
			efsw_addwatch(m_fileWatcher, realPath.generic_u8string().c_str(), FileSystemCallback, 1, this);

		for (const auto& entry : std::filesystem::recursive_directory_iterator(realPath))
		{
			if (entry.is_regular_file() && CheckExtension(entry.path().generic_u8string()))
			{
				try
				{
					RegisterModule(entry.path());
				}
				catch (const std::exception& e)
				{
					NazaraWarning("failed to register module " + entry.path().generic_u8string() + ": " + e.what());
				}
			}
		}
	}

	ShaderAst::ModulePtr FilesystemModuleResolver::Resolve(const std::string& moduleName)
	{
		auto it = m_modules.find(moduleName);
		if (it == m_modules.end())
			return {};

		return it->second;
	}

	void FilesystemModuleResolver::OnFileAdded(std::string_view directory, std::string_view filename)
	{
		if (!CheckExtension(filename))
			return;

		RegisterModule(std::filesystem::path(directory) / filename);
	}

	void FilesystemModuleResolver::OnFileRemoved(std::string_view directory, std::string_view filename)
	{
		if (!CheckExtension(filename))
			return;

		std::filesystem::path canonicalPath = std::filesystem::canonical(std::filesystem::path(directory) / filename);

		auto it = m_moduleByFilepath.find(canonicalPath.generic_u8string());
		if (it != m_moduleByFilepath.end())
		{
			m_modules.erase(it->second);
			m_moduleByFilepath.erase(it);
		}
	}

	void FilesystemModuleResolver::OnFileMoved(std::string_view directory, std::string_view filename, std::string_view oldFilename)
	{
		if (oldFilename.empty() || !CheckExtension(oldFilename))
			return;

		std::filesystem::path canonicalPath = std::filesystem::canonical(std::filesystem::path(directory) / oldFilename);
		auto it = m_moduleByFilepath.find(canonicalPath.generic_u8string());
		if (it != m_moduleByFilepath.end())
		{
			std::filesystem::path newCanonicalPath = std::filesystem::canonical(std::filesystem::path(directory) / filename);

			std::string moduleName = std::move(it->second);
			m_moduleByFilepath.erase(it);

			m_moduleByFilepath.emplace(newCanonicalPath.generic_u8string(), std::move(moduleName));
		}
	}

	void FilesystemModuleResolver::OnFileUpdated(std::string_view directory, std::string_view filename)
	{
		if (!CheckExtension(filename))
			return;

		RegisterModule(std::filesystem::path(directory) / filename);
	}
	
	bool FilesystemModuleResolver::CheckExtension(std::string_view filename)
	{
		return EndsWith(filename, ModuleExtension, Nz::CaseIndependent{}) || EndsWith(filename, CompiledModuleExtension, Nz::CaseIndependent{});
	}
}
