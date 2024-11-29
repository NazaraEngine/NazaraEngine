#include <Nazara/Core.hpp>
#include <Nazara/OpenGLRenderer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <iostream>
#include <numeric>

const char fragmentSource[] = R"(
#version 310 es

#if GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(std430) buffer OffsetTest
{
	vec4 a;
	vec3 b;
	vec2 c;
	vec4 d;
	vec3 e[10];
	mat4 f;
};

void main()
{
}
)";

const char vertexSource[] = R"(
#version 300 es

void main()
{
	gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

template <typename T, typename Compare>
std::vector<std::size_t> SortIndexes(const std::vector<T>& vec, Compare&& compare)
{
	std::vector<std::size_t> p(vec.size());
	std::iota(p.begin(), p.end(), 0);
	std::sort(p.begin(), p.end(), [&](std::size_t i, std::size_t j) { return compare(vec[i], vec[j]); });

	return p;
}

int main()
{
	Nz::Renderer::Config rendererConfig;
	rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Renderer> nazara(rendererConfig);
	if (Nz::Renderer::Instance()->QueryAPI() != Nz::RenderAPI::OpenGL)
	{
		std::cout << "This program only works with OpenGL" << std::endl;
		return EXIT_FAILURE;
	}

	std::shared_ptr<Nz::OpenGLDevice> device = std::static_pointer_cast<Nz::OpenGLDevice>(Nz::Renderer::Instance()->InstanciateRenderDevice(0));

	std::string err;

	// Fragment shader
	Nz::GL::Shader fragmentShader;
	if (!fragmentShader.Create(*device, GL_FRAGMENT_SHADER))
	{
		std::cerr << "Failed to create fragment shader" << std::endl;
		return EXIT_FAILURE;
	}

	fragmentShader.SetSource(fragmentSource, sizeof(fragmentSource));
	fragmentShader.Compile();

	if (!fragmentShader.GetCompilationStatus(&err))
	{
		std::cerr << "Failed to compile fragment shader: " << err << std::endl;
		return EXIT_FAILURE;
	}

	// Vertex shader
	Nz::GL::Shader vertexShader;
	if (!vertexShader.Create(*device, GL_VERTEX_SHADER))
	{
		std::cerr << "Failed to create vertex shader" << std::endl;
		return EXIT_FAILURE;
	}

	vertexShader.SetSource(vertexSource, sizeof(vertexSource));
	vertexShader.Compile();

	if (!vertexShader.GetCompilationStatus(&err))
	{
		std::cerr << "Failed to compile vertex shader: " << err << std::endl;
		return EXIT_FAILURE;
	}

	// Program
	Nz::GL::Program program;
	if (!program.Create(*device))
	{
		std::cerr << "Failed to create program" << std::endl;
		return EXIT_FAILURE;
	}

	program.AttachShader(fragmentShader.GetObjectId());
	program.AttachShader(vertexShader.GetObjectId());
	program.Link();

	if (!program.GetLinkStatus(&err))
	{
		std::cerr << "Failed to link program: " << err << std::endl;
		return EXIT_FAILURE;
	}

	// Get infos
	GLuint blockIndex = program.GetResourceIndex(GL_SHADER_STORAGE_BLOCK, "OffsetTest");
	if (blockIndex == GL_INVALID_INDEX)
	{
		std::cerr << "Failed to find uniform block in program" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<GLint> varIndices = program.GetActiveVariableIndices(GL_SHADER_STORAGE_BLOCK, blockIndex);

	std::vector<GLint> offsets(varIndices.size());
	for (std::size_t i = 0; i < varIndices.size(); ++i)
		program.GetResource(GL_BUFFER_VARIABLE, varIndices[i], GL_OFFSET, 1, nullptr, &offsets[i]);

	auto p = SortIndexes(offsets, std::less<std::size_t>());

	std::vector<std::size_t> computedOffsets;

	nzsl::FieldOffsets fieldOffsets(nzsl::StructLayout::Std140);
	computedOffsets.push_back(fieldOffsets.AddField(nzsl::StructFieldType::Float4));
	computedOffsets.push_back(fieldOffsets.AddField(nzsl::StructFieldType::Float3));
	computedOffsets.push_back(fieldOffsets.AddField(nzsl::StructFieldType::Float2));
	computedOffsets.push_back(fieldOffsets.AddField(nzsl::StructFieldType::Float4));
	computedOffsets.push_back(fieldOffsets.AddFieldArray(nzsl::StructFieldType::Float3, 10));
	computedOffsets.push_back(fieldOffsets.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true));


	GLint dataSize;
	program.GetResource(GL_SHADER_STORAGE_BLOCK, blockIndex, GL_BUFFER_DATA_SIZE, 1, nullptr, &dataSize);

	if (fieldOffsets.GetAlignedSize() != std::size_t(dataSize))
		std::cout << "size mismatch (computed " << fieldOffsets.GetAlignedSize() << ", reference has " << dataSize << ")" << std::endl;

	if (computedOffsets.size() != varIndices.size())
	{
		std::cout << "member count mismatch" << std::endl;
		return EXIT_FAILURE;
	}

	for (std::size_t i = 0; i < varIndices.size(); ++i)
	{
		GLint realOffset = offsets[p[i]];
		std::cout << program.GetResourceName(GL_BUFFER_VARIABLE, varIndices[p[i]]) << ": " << realOffset;
		if (realOffset != computedOffsets[i])
			std::cout << " ERR";

		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}
