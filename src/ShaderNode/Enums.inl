#include <ShaderNode/Enums.hpp>

template<typename T>
std::optional<T> DecodeEnum(const std::string_view& str)
{
	constexpr std::size_t ValueCount = static_cast<std::size_t>(T::Max) + 1;
	for (std::size_t i = 0; i < ValueCount; ++i)
	{
		T value = static_cast<T>(i);
		if (str == EnumToString(value))
			return value;
	}

	return {};
}
