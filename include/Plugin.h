#pragma once

#include <string_view>

namespace Plugin
{
	using namespace std::literals;

	inline constexpr REL::Version VERSION
	{
		// clang-format off
		1u,
		0u,
		0u,
		// clang-format on
	};

	inline constexpr auto NAME = "Rings"sv;
	inline constexpr auto AUTHOR = "Zebrina"sv;
	inline constexpr auto PLUGINFILE = "Rings.esl"sv;
	inline constexpr auto CONFIGFILE = "Data\\SKSE\\Plugins\\Rings.toml"sv;
}