// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_MTLPARSER_HPP
#define NAZARA_FORMATS_MTLPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <unordered_map>

namespace Nz
{
	class NAZARA_UTILITY_API MTLParser
	{
		public:
			struct Material;

			MTLParser() = default;
			~MTLParser() = default;

			inline Material* AddMaterial(const String& matName);

			inline void Clear();

			inline const Material* GetMaterial(const String& materialName) const;
			inline const std::unordered_map<String, Material>& GetMaterials() const;

			bool Parse(Stream& stream);

			bool Save(Stream& stream) const;

			struct Material
			{
				Color ambient = Color::White;
				Color diffuse = Color::White;
				Color specular = Color::White;
				String alphaMap;
				String ambientMap;
				String bumpMap;
				String decalMap;
				String diffuseMap;
				String displacementMap;
				String reflectionMap;
				String shininessMap;
				String specularMap;
				float alpha = 1.f;
				float refractionIndex = 1.f;
				float shininess = 1.f;
				unsigned int illumModel = 0;
			};

		private:
			bool Advance(bool required = true);
			template<typename T> void Emit(const T& text) const;
			inline void EmitLine() const;
			template<typename T> void EmitLine(const T& line) const;
			inline void Error(const String& message);
			inline void Flush() const;
			inline void Warning(const String& message);
			inline void UnrecognizedLine(bool error = false);

			std::unordered_map<String, Material> m_materials;
			mutable Stream* m_currentStream;
			String m_currentLine;
			mutable StringStream m_outputStream;
			bool m_keepLastLine;
			unsigned int m_lineCount;
			unsigned int m_streamFlags;
	};
}

#include <Nazara/Utility/Formats/MTLParser.inl>

#endif // NAZARA_FORMATS_MTLPARSER_HPP
