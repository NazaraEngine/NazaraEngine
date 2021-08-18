#include <Nazara/Core.hpp>
#include <Nazara/OpenGLRenderer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility/FieldOffsets.hpp>
#include <iostream>
#include <numeric>

const char fragmentSource[] = R"(
#version 310 es

#if GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(binding = 3, std140) uniform LightParameters
{
	mat4 projectionMatrix;
	mat4 invProjectionMatrix;
	mat4 viewMatrix;
	mat4 invViewMatrix;
	mat4 viewProjMatrix;
	mat4 invViewProjMatrix;
	vec2 renderTargetSize;
	vec2 invRenderTargetSize;
	vec3 eyePosition;
};

void main()
{
}
)";

const char vertexSource[] = R"(
#version 310 es

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
	GLuint blockIndex = program.GetUniformBlockIndex("LightParameters");
	if (blockIndex == GL_INVALID_INDEX)
	{
		std::cerr << "Failed to find uniform block in program" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<GLint> uniformIndices = program.GetActiveUniformBlockUniformIndices(blockIndex);

	std::vector<GLint> offsets = program.GetActiveUniforms(GLsizei(uniformIndices.size()), reinterpret_cast<GLuint*>(uniformIndices.data()), GL_UNIFORM_OFFSET);

	auto p = SortIndexes(offsets, std::less<std::size_t>());

	std::vector<std::size_t> computedOffsets;
	
	Nz::FieldOffsets fieldOffsets(Nz::StructLayout::Std140);
	computedOffsets.push_back(fieldOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true));
	computedOffsets.push_back(fieldOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true));
	computedOffsets.push_back(fieldOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true));
	computedOffsets.push_back(fieldOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true));
	computedOffsets.push_back(fieldOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true));
	computedOffsets.push_back(fieldOffsets.AddMatrix(Nz::StructFieldType::Float1, 4, 4, true));
	computedOffsets.push_back(fieldOffsets.AddField(Nz::StructFieldType::Float2));
	computedOffsets.push_back(fieldOffsets.AddField(Nz::StructFieldType::Float2));
	computedOffsets.push_back(fieldOffsets.AddField(Nz::StructFieldType::Float3));


	GLint dataSize;
	program.GetActiveUniformBlock(blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);

	if (fieldOffsets.GetAlignedSize() != dataSize)
		std::cout << "size mismatch (computed " << fieldOffsets.GetAlignedSize() << ", reference has " << dataSize << ")" << std::endl;;

	if (computedOffsets.size() != uniformIndices.size())
	{
		std::cout << "member count mismatch" << std::endl;
		return EXIT_FAILURE;
	}

	for (std::size_t i = 0; i < uniformIndices.size(); ++i)
	{
		GLint realOffset = offsets[p[i]];
		std::cout << program.GetActiveUniformName(uniformIndices[p[i]]) << ": " << realOffset;
		if (realOffset != computedOffsets[i])
			std::cout << " ERR";

		std::cout << std::endl;
	}

	return EXIT_SUCCESS;
}
