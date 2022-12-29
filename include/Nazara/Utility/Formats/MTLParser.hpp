// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_FORMATS_MTLPARSER_HPP
#define NAZARA_UTILITY_FORMATS_MTLPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
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

			inline Material* AddMaterial(const std::string& matName);

			inline void Clear();

			inline const Material* GetMaterial(const std::string& materialName) const;
			inline const std::unordered_map<std::string, Material>& GetMaterials() const;

			bool Parse(Stream& stream);

			bool Save(Stream& stream) const;

			struct Material
			{
				Color ambient = Color::White();
				Color diffuse = Color::White();
				Color specular = Color::White();
				std::string alphaMap;
				std::string ambientMap;
				std::string bumpMap;
				std::string decalMap;
				std::string diffuseMap;
				std::string displacementMap;
				std::string emissiveMap; //< <!> Custom addition: not present in MTL
				std::string normalMap; //< <!> Custom addition: not present in MTL
				std::string reflectionMap;
				std::string shininessMap;
				std::string specularMap;
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
			inline void Error(const std::string& message);
			inline void Flush() const;
			inline void Warning(const std::string& message);
			inline void UnrecognizedLine(bool error = false);

			std::unordered_map<std::string, Material> m_materials;
			mutable Stream* m_currentStream;
			std::string m_currentLine;
			mutable std::ostringstream m_outputStream;
			bool m_keepLastLine;
			unsigned int m_lineCount;
	};
}

#include <Nazara/Utility/Formats/MTLParser.inl>

#endif // NAZARA_UTILITY_FORMATS_MTLPARSER_HPP
