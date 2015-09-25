// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_MTLPARSER_HPP
#define NAZARA_FORMATS_MTLPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <unordered_map>

namespace Nz
{
	class NAZARA_UTILITY_API MTLParser
	{
		public:
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

			MTLParser(InputStream& stream$);
			~MTLParser();

			const Material* GetMaterial(const String& materialName) const;
			const std::unordered_map<String, Material>& GetMaterials() const;

			bool Parse();

		private:
			bool Advance(bool required = true);
			void Error(const String& message);
			void Warning(const String& message);
			void UnrecognizedLine(bool error = false);

			std::unordered_map<String, Material> m_materials;
			InputStream& m_stream;
			String m_currentLine;
			bool m_keepLastLine;
			unsigned int m_lineCount;
			unsigned int m_streamFlags;
	};
}

#endif // NAZARA_FORMATS_MTLPARSER_HPP
