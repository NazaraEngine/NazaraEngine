// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline void Material::AddPass(std::size_t passIndex, std::shared_ptr<MaterialPass> pass)
	{
		if (passIndex >= m_passes.size())
			m_passes.resize(passIndex + 1);

		m_passes[passIndex] = std::move(pass);
	}

	inline std::size_t Material::FindTextureByTag(const std::string& tag) const
	{
		auto it = m_textureByTag.find(tag);
		if (it == m_textureByTag.end())
			return InvalidIndex;

		return it->second;
	}

	inline std::size_t Material::FindUniformByTag(const std::string& tag) const
	{
		auto it = m_uniformBlockByTag.find(tag);
		if (it == m_uniformBlockByTag.end())
			return InvalidIndex;

		return it->second;
	}

	template<typename F>
	void Material::ForEachPass(F&& callback)
	{
		for (std::size_t i = 0; i < m_passes.size(); ++i)
		{
			if (m_passes[i])
				callback(i, m_passes[i]);
		}
	}

	inline const std::shared_ptr<MaterialPass>& Material::GetPass(std::size_t passIndex) const
	{
		if (passIndex >= m_passes.size())
		{
			static std::shared_ptr<MaterialPass> dummy;
			return dummy;
		}

		return m_passes[passIndex];
	}

	inline const std::shared_ptr<RenderPipelineLayout>& Material::GetRenderPipelineLayout() const
	{
		return m_renderPipelineLayout;
	}

	inline const MaterialSettings& Material::GetSettings() const
	{
		return m_settings;
	}

	inline auto Material::GetTextureData(std::size_t textureIndex) const -> const TextureData&
	{
		assert(textureIndex < m_textures.size());
		return m_textures[textureIndex];
	}

	inline std::size_t Material::GetTextureCount() const
	{
		return m_textures.size();
	}

	inline auto Material::GetUniformBlockData(std::size_t uniformBlockIndex) const -> const UniformBlockData&
	{
		assert(uniformBlockIndex < m_uniformBlocks.size());
		return m_uniformBlocks[uniformBlockIndex];
	}

	inline std::size_t Material::GetUniformBlockCount() const
	{
		return m_uniformBlocks.size();
	}

	inline bool Material::HasPass(std::size_t passIndex) const
	{
		if (passIndex >= m_passes.size())
			return false;

		return m_passes[passIndex] != nullptr;
	}

	inline void Material::RemovePass(std::size_t passIndex)
	{
		if (passIndex >= m_passes.size())
			return;

		m_passes[passIndex].reset();
	}
	
	inline ImageType Material::ToImageType(nzsl::ImageType imageType)
	{
		switch (imageType)
		{
			case nzsl::ImageType::E1D:       return ImageType::E1D;
			case nzsl::ImageType::E1D_Array: return ImageType::E1D_Array;
			case nzsl::ImageType::E2D:       return ImageType::E2D;
			case nzsl::ImageType::E2D_Array: return ImageType::E2D_Array;
			case nzsl::ImageType::E3D:       return ImageType::E3D;
			case nzsl::ImageType::Cubemap:   return ImageType::Cubemap;
		}

		NazaraError("invalid image type 0x" + NumberToString(UnderlyingCast(imageType), 16));
		return ImageType::E2D;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
