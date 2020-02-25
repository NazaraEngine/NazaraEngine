// Copyright (C) 2016 Jérôme Leclercq, Arnaud Cadot
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/LuaBinding.hpp>
#include <NDK/LuaAPI.hpp>

namespace Ndk
{
	/*!
	* \brief Binds Renderer module to Lua
	*/
	void LuaBinding::BindRenderer()
	{
		/*********************************** Nz::Texture ***********************************/
		texture.Inherit<Nz::AbstractImageRef>(abstractImage, [] (Nz::TextureRef* textureRef) -> Nz::AbstractImageRef*
		{
			return reinterpret_cast<Nz::AbstractImageRef*>(textureRef); //TODO: Make a ObjectRefCast
		});

		texture.SetConstructor([] (Nz::LuaInstance& /*lua*/, Nz::TextureRef* instance, std::size_t /*argumentCount*/)
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

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the Renderer classes
	*/
	void LuaBinding::RegisterRenderer(Nz::LuaInstance& instance)
	{
		texture.Register(instance);
	}
}
