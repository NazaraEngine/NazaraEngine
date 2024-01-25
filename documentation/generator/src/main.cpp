#include <Nazara/Core/Clock.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <cppast/libclang_parser.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_expression.hpp>
#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_member_variable.hpp>
#include <cppast/cpp_enum.hpp>
#include <cppast/visitor.hpp>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

nlohmann::ordered_json buildClass(const std::string& scope, const cppast::cpp_class& classNode);
nlohmann::ordered_json buildEnum(const std::string& scope, const cppast::cpp_enum& enumNode);

class simpleCodeGenerator : public cppast::code_generator
{
	std::string str_;                 // the result
	bool        was_newline_ = false; // whether or not the last token was a newline
	// needed for lazily printing them

public:
	simpleCodeGenerator(const cppast::cpp_entity& e)
	{
		// kickoff code generation here
		cppast::generate_code(*this, e);
	}

	simpleCodeGenerator(const cppast::cpp_expression& e)
	{
		struct dummy_entity : public cppast::cpp_entity
		{
			using cpp_entity::cpp_entity;

			cppast::cpp_entity_kind do_get_entity_kind() const noexcept override
			{
				return cppast::cpp_entity_kind::unexposed_t;
			}
		};

		dummy_entity dummy("dummy");
		output output(type_safe::ref(*this), type_safe::ref(dummy), cppast::cpp_access_specifier_kind::cpp_public);
		cppast::detail::write_expression(output, e);
	}

	// return the result
	const std::string& str() const noexcept
	{
		return str_;
	}

private:
	// called to retrieve the generation options of an entity
	generation_options do_get_options(const cppast::cpp_entity&,
										cppast::cpp_access_specifier_kind) override
	{
		// generate declaration only
		return code_generator::declaration;
	}

	// no need to handle indentation, as only a single line is used
	void do_indent() override {}
	void do_unindent() override {}

	// called when a generic token sequence should be generated
	// there are specialized callbacks for various token kinds,
	// to e.g. implement syntax highlighting
	void do_write_token_seq(cppast::string_view tokens) override
	{
		if (was_newline_)
		{
			// lazily append newline as space
			str_ += ' ';
			was_newline_ = false;
		}
		// append tokens
		str_ += tokens.c_str();
	}

	// called when a newline should be generated
	// we're lazy as it will always generate a trailing newline,
	// we don't want
	void do_write_newline() override
	{
		was_newline_ = true;
	}

};

int main()
{
	nlohmann::ordered_json outputDoc;
	nlohmann::ordered_json& moduleDoc = outputDoc["modules"];

	cppast::libclang_compilation_database database("compile_commands");

	cppast::libclang_compile_config config;

	Nz::MillisecondClock time;

	for (auto&& entry : std::filesystem::directory_iterator("../include/Nazara"))
	{
		if (!entry.is_directory())
			continue;

		std::string moduleName = Nz::PathToString(entry.path().filename());
		std::cout << "Parsing " << moduleName << " headers..." << std::endl;

		nlohmann::ordered_json& moduleEntryDoc = moduleDoc[moduleName];

		// Use module source file as flags reference to parse this module headers
		cppast::libclang_compile_config config(database, "src/Nazara/" + moduleName + "/" + moduleName + ".cpp");
		config.define_macro("NAZARA_DOCGEN", "");

		cppast::stderr_diagnostic_logger logger;
		//logger.set_verbose(true);

		std::vector<std::thread> threads;
		std::mutex jsonMutex;

		for (auto&& headerFile : std::filesystem::recursive_directory_iterator(entry.path()))
		{
			if (!headerFile.is_regular_file() || Nz::PathToString(headerFile.path().extension()) != ".hpp")
				continue;

			std::string filepath = Nz::PathToString(headerFile.path());
			threads.push_back(std::thread([&, filepath]
			{
				std::cout << " - Parsing " + filepath + "...\n";

				// the entity index is used to resolve cross references in the AST
				// we don't need that, so it will not be needed afterwards
				cppast::cpp_entity_index idx;
				// the parser is used to parse the entity
				// there can be multiple parser implementations
				cppast::libclang_parser parser(type_safe::ref(logger));
				// parse the file
				try
				{
					auto file = parser.parse(idx, filepath, config);
					if (parser.error())
					{
						std::cerr << "failed to parse " << filepath << "\n";
						return;
					}

					std::vector<std::string> prefixes;
					auto prefix = [&]
					{
						std::string p;
						for (const std::string& prefix : prefixes)
						{
							p += prefix;
							p += "::";
						}

						return p;
					};

					// visit each entity in the file
					bool insideNazaraNamespace = false;
					cppast::visit(*file, [&](const cppast::cpp_entity& e, cppast::visitor_info info)
					{
						if (info.event == cppast::visitor_info::container_entity_enter)
						{
							if (e.kind() == cppast::cpp_entity_kind::file_t)
								return true;

							if (!insideNazaraNamespace)
							{
								if (e.kind() != cppast::cpp_entity_kind::namespace_t)
									return false;

								if (!prefixes.empty() || e.name() != "Nz")
									return false;

								insideNazaraNamespace = true;
							}

							bool shouldEnter = true;
							switch (e.kind())
							{
								case cppast::cpp_entity_kind::class_t:
								{
									auto& classNode = static_cast<const cppast::cpp_class&>(e);
									if (!classNode.is_definition())
									{
										shouldEnter = false;
										break;
									}

									std::cout << "found " << cppast::to_string(classNode.class_kind()) << " " << prefix() << e.name() << std::endl;
									nlohmann::ordered_json classDoc = buildClass(prefix(), classNode);
									std::unique_lock lock(jsonMutex);
									moduleEntryDoc["classes"].push_back(std::move(classDoc));
									break;
								}

								case cppast::cpp_entity_kind::enum_t:
								{
									auto& enumNode = static_cast<const cppast::cpp_enum&>(e);
									if (!enumNode.is_definition())
									{
										shouldEnter = false;
										break;
									}

									std::cout << "found " << (enumNode.is_scoped() ? "enum class" : "enum") << " " << prefix() << e.name() << std::endl;
									nlohmann::ordered_json enumDoc = buildEnum(prefix(), enumNode);
									std::unique_lock lock(jsonMutex);
									moduleEntryDoc["enums"].push_back(std::move(enumDoc));
									break;
								}

								default:
									break;
							}

							prefixes.push_back(e.name());
							return shouldEnter;
						}
						else if (info.event == cppast::visitor_info::container_entity_exit) // exiting an old container
						{
							if (!prefixes.empty())
							{
								prefixes.pop_back();
								if (prefixes.empty())
									insideNazaraNamespace = false;
							}
						}

						return true;
					});
				}
				catch (const cppast::libclang_error& err)
				{
					std::cerr << "failed to parse " << filepath << ": " << err.what() << "\n";
				}
			}));
		}

		for (std::thread& t : threads)
			t.join();

		auto& classArray = moduleEntryDoc["classes"];

		std::sort(classArray.begin(), classArray.end(), [](const nlohmann::ordered_json& classA, const nlohmann::ordered_json& classB)
		{
			const std::string& nameA = classA["name"];
			const std::string& nameB = classB["name"];
			return nameA < nameB;
		});
	}

	std::fstream outputFile("docgen.json", outputFile.trunc | outputFile.out);
	outputFile << outputDoc.dump(1, '\t');

	std::cout << "Generated documentation in " << time.GetElapsedTime() << std::endl;

	return EXIT_SUCCESS;
}

nlohmann::ordered_json buildClass(const std::string& scope, const cppast::cpp_class& classNode)
{
	nlohmann::ordered_json classDoc;
	classDoc["name"] = scope + classNode.name();

	bool isPublic = classNode.class_kind() != cppast::cpp_class_kind::class_t;
	for (const auto& e : classNode)
	{
		switch (e.kind())
		{
			case cppast::cpp_entity_kind::access_specifier_t:
			{
				isPublic = static_cast<const cppast::cpp_access_specifier&>(e).access_specifier() == cppast::cpp_access_specifier_kind::cpp_public;
				break;
			}

			case cppast::cpp_entity_kind::constructor_t:
			{
				if (!isPublic)
					break;

				const auto& memberFunc = static_cast<const cppast::cpp_member_function&>(e);

				auto& constructorDoc = classDoc["constructors"].emplace_back();
				constructorDoc["name"] = memberFunc.name();
				constructorDoc["signature"] = memberFunc.signature();
				constructorDoc["codeGen"] = simpleCodeGenerator(memberFunc).str();

				auto& parameterArray = constructorDoc["parameters"];
				parameterArray = nlohmann::ordered_json::array();

				for (const auto& parameter : memberFunc.parameters())
				{
					auto& parameterDoc = parameterArray.emplace_back();
					parameterDoc["name"] = parameter.name();
					parameterDoc["type"] = cppast::to_string(parameter.type());
					if (const auto& defaultOpt = parameter.default_value())
						parameterDoc["default"] = simpleCodeGenerator(defaultOpt.value()).str();
				}
				break;
			}

			case cppast::cpp_entity_kind::destructor_t:
				break;

			case cppast::cpp_entity_kind::member_function_t:
			{
				if (!isPublic)
					break;

				const auto& memberFunc = static_cast<const cppast::cpp_member_function&>(e);

				auto& methodDoc = classDoc["methods"].emplace_back();
				methodDoc["name"] = memberFunc.name();
				methodDoc["signature"] = memberFunc.signature();
				methodDoc["codeGen"] = simpleCodeGenerator(memberFunc).str();
				methodDoc["returnType"] = cppast::to_string(memberFunc.return_type());

				auto& parameterArray = methodDoc["parameters"];
				parameterArray = nlohmann::ordered_json::array();

				for (const auto& parameter : memberFunc.parameters())
				{
					auto& parameterDoc = parameterArray.emplace_back();
					parameterDoc["name"] = parameter.name();
					parameterDoc["type"] = cppast::to_string(parameter.type());
					if (const auto& defaultOpt = parameter.default_value())
						parameterDoc["default"] = simpleCodeGenerator(defaultOpt.value()).str();
				}
				break;
			}

			case cppast::cpp_entity_kind::member_variable_t:
			{
				if (!isPublic)
					break;

				const auto& memberVariable = static_cast<const cppast::cpp_member_variable&>(e);
				
				auto& variableDoc = classDoc["variables"].emplace_back();
				variableDoc["name"] = memberVariable.name();
				variableDoc["type"] = cppast::to_string(memberVariable.type());
				break;
			}

			case cppast::cpp_entity_kind::function_t:
			{
				if (!isPublic)
					break;

				const auto& memberFunc = static_cast<const cppast::cpp_function&>(e);

				auto& methodDoc = classDoc["staticMethods"].emplace_back();
				methodDoc["name"] = memberFunc.name();
				methodDoc["signature"] = memberFunc.signature();
				methodDoc["codeGen"] = simpleCodeGenerator(memberFunc).str();
				methodDoc["returnType"] = cppast::to_string(memberFunc.return_type());

				auto& parameterArray = methodDoc["parameters"];
				parameterArray = nlohmann::ordered_json::array();

				for (const auto& parameter : memberFunc.parameters())
				{
					auto& parameterDoc = parameterArray.emplace_back();
					parameterDoc["name"] = parameter.name();
					parameterDoc["type"] = cppast::to_string(parameter.type());
					if (const auto& defaultOpt = parameter.default_value())
						parameterDoc["default"] = simpleCodeGenerator(defaultOpt.value()).str();
				}
				break;
			}

			default:
			{
				if (isPublic)
					std::cerr << "ignored public " << cppast::to_string(e.kind()) << " " << e.name() << std::endl;

				break;
			}
		}
	}

	return classDoc;
}

nlohmann::ordered_json buildEnum(const std::string& scope, const cppast::cpp_enum& enumNode)
{
	nlohmann::ordered_json enumDoc;
	enumDoc["name"] = scope + enumNode.name();
	enumDoc["scoped"] = enumNode.is_scoped();
	if (enumNode.has_explicit_type())
		enumDoc["underlying_type"] = cppast::to_string(enumNode.underlying_type());

	for (const auto& entry : enumNode)
	{
		auto& valueDoc = enumDoc["values"].emplace_back();
		valueDoc["name"] = entry.name();
		if (const auto& exprOpt = entry.value())
			valueDoc["value"] = simpleCodeGenerator(exprOpt.value()).str();
	}

	return enumDoc;
}

