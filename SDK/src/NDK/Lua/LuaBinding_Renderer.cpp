// Copyright (C) 2017 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Lua/LuaBinding_Renderer.hpp>
#include <NDK/LuaAPI.hpp>
#include <NDK/Lua/LuaBinding.hpp>
#include <NDK/Lua/LuaBinding_Utility.hpp>

namespace Ndk
{
	std::unique_ptr<LuaBinding_Base> LuaBinding_Base::BindRenderer(LuaBinding& binding)
	{
		return std::make_unique<LuaBinding_Renderer>(binding);
	}

	LuaBinding_Renderer::LuaBinding_Renderer(LuaBinding& binding) :
	LuaBinding_Base(binding)
	{
		LuaBinding_Utility& utility = static_cast<LuaBinding_Utility&>(*m_binding.utility);

		/*********************************** Nz::Texture ***********************************/
		texture.Reset("Texture");
		{
			texture.Inherit<Nz::AbstractImageRef>(utility.abstractImage, [] (Nz::TextureRef* textureRef) -> Nz::AbstractImageRef*
			{
				return reinterpret_cast<Nz::AbstractImageRef*>(textureRef); //TODO: Make a ObjectRefCast
			});

			texture.SetConstructor([] (Nz::LuaState& /*lua*/, Nz::TextureRef* instance, std::size_t /*argumentCount*/)
			{
				Nz::PlacementNew(instance, Nz::Texture::New());
				return true;
			});

			texture.BindMethod("Create", &Nz::Texture::Create, static_cast<Nz::UInt8>(1), 1U);
			texture.BindMethod("Destroy", &Nz::Texture::Destroy);

			//texture.BindMethod("Download", &Nz::Texture::Download);

			texture.BindMethod("EnableMipmapping", &Nz::Texture::EnableMipmapping);
			texture.BindMethod("EnsureMipmapsUpdate", &Nz::Texture::EnsureMipmapsUpdate);
			texture.BindMethod("HasMipmaps", &Nz::Texture::HasMipmaps);
			texture.BindMethod("InvalidateMipmaps", &Nz::Texture::InvalidateMipmaps);
			texture.BindMethod("IsValid", &Nz::Texture::IsValid);

			texture.BindMethod("LoadFromFile", &Nz::Texture::LoadFromFile, true, Nz::ImageParams());
			//bool LoadFromImage(const Image& image, bool generateMipmaps = true);
			//bool LoadFromMemory(const void* data, std::size_t size, const ImageParams& params = ImageParams(), bool generateMipmaps = true);
			//bool LoadFromStream(Stream& stream, const ImageParams& params = ImageParams(), bool generateMipmaps = true);

			texture.BindMethod("LoadArrayFromFile", &Nz::Texture::LoadArrayFromFile, Nz::Vector2ui(2, 2), true, Nz::ImageParams());
			//bool LoadArrayFromImage(const Image& image, bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));
			//bool LoadArrayFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));
			//bool LoadArrayFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const Vector2ui& atlasSize = Vector2ui(2, 2));

			//bool LoadCubemapFromFile(const String& filePath, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const CubemapParams& cubemapParams = CubemapParams());
			//bool LoadCubemapFromImage(const Image& image, bool generateMipmaps = true, const CubemapParams& params = CubemapParams());
			//bool LoadCubemapFromMemory(const void* data, std::size_t size, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const CubemapParams& cubemapParams = CubemapParams());
			//bool LoadCubemapFromStream(Stream& stream, const ImageParams& imageParams = ImageParams(), bool generateMipmaps = true, const CubemapParams& cubemapParams = CubemapParams());

			texture.BindMethod("LoadFaceFromFile", &Nz::Texture::LoadFaceFromFile, Nz::ImageParams());
			//bool LoadFaceFromMemory(CubemapFace face, const void* data, std::size_t size, const ImageParams& params = ImageParams());
			//bool LoadFaceFromStream(CubemapFace face, Stream& stream, const ImageParams& params = ImageParams());

			texture.BindMethod("SaveToFile", &Nz::Texture::SaveToFile, Nz::ImageParams());
			//bool SaveToStream(Stream& stream, const String& format, const ImageParams& params = ImageParams());

			texture.BindMethod("SetMipmapRange", &Nz::Texture::SetMipmapRange);

			texture.BindStaticMethod("IsFormatSupported", &Nz::Texture::IsFormatSupported);
			texture.BindStaticMethod("IsMipmappingSupported", &Nz::Texture::IsMipmappingSupported);
			texture.BindStaticMethod("IsTypeSupported", &Nz::Texture::IsTypeSupported);
		}

		/*********************************** Nz::TextureLibrary ***********************************/
		textureLibrary.Reset("TextureLibrary");
		{
			textureLibrary.BindStaticMethod("Get", &Nz::TextureLibrary::Get);
			textureLibrary.BindStaticMethod("Has", &Nz::TextureLibrary::Has);
			textureLibrary.BindStaticMethod("Register", &Nz::TextureLibrary::Register);
			textureLibrary.BindStaticMethod("Query", &Nz::TextureLibrary::Query);
			textureLibrary.BindStaticMethod("Unregister", &Nz::TextureLibrary::Unregister);
		}

		/*********************************** Nz::TextureManager ***********************************/
		textureManager.Reset("textureManager");
		{
			textureManager.BindStaticMethod("Clear", &Nz::TextureManager::Clear);
			textureManager.BindStaticMethod("Get", &Nz::TextureManager::Get);
			textureManager.BindStaticMethod("GetDefaultParameters", &Nz::TextureManager::GetDefaultParameters);
			textureManager.BindStaticMethod("Purge", &Nz::TextureManager::Purge);
			textureManager.BindStaticMethod("Register", &Nz::TextureManager::Register);
			textureManager.BindStaticMethod("SetDefaultParameters", &Nz::TextureManager::SetDefaultParameters);
			textureManager.BindStaticMethod("Unregister", &Nz::TextureManager::Unregister);
		}
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Renderer classes
	*/
	void LuaBinding_Renderer::Register(Nz::LuaState& state)
	{
		texture.Register(state);
		textureLibrary.Register(state);
		textureManager.Register(state);
	}
}
