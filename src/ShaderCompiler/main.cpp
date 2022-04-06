#include <Nazara/Core/File.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Shader/LangWriter.hpp>
#include <Nazara/Shader/ShaderLangErrors.hpp>
#include <Nazara/Shader/ShaderLangLexer.hpp>
#include <Nazara/Shader/ShaderLangParser.hpp>
#include <Nazara/Shader/Ast/AstSerializer.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <cxxopts.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <filesystem>
#include <sstream>
#include <stdexcept>

std::vector<Nz::UInt8> ReadFileContent(const std::filesystem::path& filePath)
{
	Nz::File file(filePath);
	if (!file.Open(Nz::OpenMode::ReadOnly | Nz::OpenMode::Text))
		throw std::runtime_error("failed to open " + filePath.generic_u8string());

	std::size_t length = static_cast<std::size_t>(file.GetSize());
	if (length == 0)
		return {};

	std::vector<Nz::UInt8> content(length);
	if (file.Read(&content[0], length) != length)
		throw std::runtime_error("failed to read " + filePath.generic_u8string());

	return content;
}

std::string ReadSourceFileContent(const std::filesystem::path& filePath)
{
	std::vector<Nz::UInt8> content = ReadFileContent(filePath);
	return std::string(reinterpret_cast<const char*>(&content[0]), content.size());
}

void WriteFileContent(std::filesystem::path& filePath, const void* data, std::size_t size)
{
	Nz::File file(filePath);
	if (!file.Open(Nz::OpenMode::WriteOnly | Nz::OpenMode::Truncate))
		throw std::runtime_error("failed to open " + filePath.generic_u8string());

	if (file.Write(data, size) != size)
		throw std::runtime_error("failed to write " + filePath.generic_u8string());
}

int main(int argc, char* argv[])
{
	cxxopts::Options options("nzslc", "Tool for validating and compiling NZSL shaders");
	options.add_options()
		("c,compile", "Compile input shader")
		("output-nzsl", "Output shader as NZSL to stdout")
		("header-file", "Generate an includable header file")
		("i,input", "Input file(s)", cxxopts::value<std::string>())
		("o,output", "Output path", cxxopts::value<std::string>()->default_value("."), "path")
		("p,partial", "Allow partial compilation")
		("s,show", "Show informations about the shader (default)")
		("h,help", "Print usage")
	;

	options.parse_positional("input");
	options.positional_help("shader path");

	try
	{
		auto result = options.parse(argc, argv);
		if (result.count("help") > 0)
		{
			fmt::print("{}\n", options.help());
			return EXIT_SUCCESS;
		}

		if (result.count("input") == 0)
		{
			fmt::print("no input file\n{}\n", options.help());
			return EXIT_SUCCESS;
		}

		std::filesystem::path inputPath = result["input"].as<std::string>();
		if (!std::filesystem::is_regular_file(inputPath))
		{
			fmt::print("{} is not a file\n", inputPath.generic_u8string());
			return EXIT_FAILURE;
		}

		try
		{
			Nz::ShaderAst::ModulePtr shaderModule;
			if (inputPath.extension() == ".nzsl")
			{
				std::string sourceContent = ReadSourceFileContent(inputPath);

				std::vector<Nz::ShaderLang::Token> tokens = Nz::ShaderLang::Tokenize(sourceContent, inputPath.generic_u8string());

				shaderModule = Nz::ShaderLang::Parse(tokens);
			}
			else if (inputPath.extension() == ".nzslb")
			{
				std::vector<Nz::UInt8> sourceContent = ReadFileContent(inputPath);

				shaderModule = Nz::ShaderAst::UnserializeShader(sourceContent.data(), sourceContent.size());
			}
			else
			{
				fmt::print("{} has unknown extension\n", inputPath.generic_u8string());
				return EXIT_FAILURE;
			}

			if (result.count("compile") > 0)
			{
				Nz::ShaderAst::SanitizeVisitor::Options sanitizeOptions;
				sanitizeOptions.allowPartialSanitization = result.count("partial") > 0;

				shaderModule = Nz::ShaderAst::Sanitize(*shaderModule, sanitizeOptions);

				Nz::ByteArray shaderData = Nz::ShaderAst::SerializeShader(shaderModule);

				std::filesystem::path outputPath = inputPath;
				if (result.count("header-file") > 0)
				{
					outputPath.replace_extension(".nzslb.h");

					std::stringstream ss;

					bool first = true;
					for (std::size_t i = 0; i < shaderData.size(); ++i)
					{
						if (!first)
							ss << ',';

						ss << +shaderData[i];

						first = false;
					}

					std::string headerFile = std::move(ss).str();
					WriteFileContent(outputPath, headerFile.data(), headerFile.size());
				}
				else
				{
					outputPath.replace_extension(".nzslb");
					WriteFileContent(outputPath, shaderData.GetConstBuffer(), shaderData.GetSize());
				}
			}

			if (result.count("output-nzsl") > 0)
			{
				Nz::LangWriter nzslWriter;
				fmt::print("{}", nzslWriter.Generate(*shaderModule));
			}
		}
		catch (const Nz::ShaderLang::Error& error)
		{
			fmt::print(stderr, (fmt::emphasis::bold | fg(fmt::color::red)), "{}\n", error.what());

			Nz::ShaderLang::SourceLocation errorLocation = error.GetSourceLocation();
			if (errorLocation.IsValid())
			{
				try
				{
					// Retrieve line
					std::string sourceContent = ReadSourceFileContent(*errorLocation.file);

					std::size_t lineStartOffset = 0;
					if (errorLocation.startLine > 1)
					{
						lineStartOffset = sourceContent.find('\n') + 1;
						for (std::size_t i = 0; i < errorLocation.startLine - 2; ++i) //< remember startLine is 1-based
						{
							lineStartOffset = sourceContent.find('\n', lineStartOffset);
							if (lineStartOffset == std::string::npos)
								throw std::runtime_error("file content doesn't match original source");

							++lineStartOffset;
						}
					}
					std::size_t lineEndOffset = sourceContent.find('\n', lineStartOffset);

					std::string errorLine = sourceContent.substr(lineStartOffset, lineEndOffset - lineStartOffset);

					// handle tabs
					Nz::UInt32 startColumn = errorLocation.startColumn - 1;
					std::size_t startPos = 0;
					while ((startPos = errorLine.find("\t", startPos)) != std::string::npos)
					{
						if (startPos < startColumn)
							startColumn += 3;

						errorLine.replace(startPos, 1, "    ");
						startPos += 4;
					}

					std::size_t columnSize;
					if (errorLocation.startLine == errorLocation.endLine)
						columnSize = errorLocation.endColumn - errorLocation.startColumn + 1;
					else
						columnSize = 1;

					std::string lineStr = std::to_string(errorLocation.startLine);

					fmt::print(stderr, " {} | {}\n", lineStr, errorLine);
					fmt::print(stderr, " {} | {}", std::string(lineStr.size(), ' '), std::string(startColumn, ' '));
					fmt::print(stderr, fg(fmt::color::green), "{}\n", std::string(columnSize, '^'));
				}
				catch (const std::exception& e)
				{
					fmt::print(stderr, "failed to print error line: {}\n", e.what());
				}
			}
		}
	}
	catch (const cxxopts::OptionException& e)
	{
		fmt::print(stderr, "{}\n{}\n", e.what(), options.help());
	}
	catch (const std::exception& e)
	{
		fmt::print(stderr, "{}\n", e.what());
	}

	return EXIT_SUCCESS;
}
