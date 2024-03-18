#pragma once

//#define BETTER_SAFE_THAN_SORRY

#pragma warning(push)
#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>

#ifndef NDEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

using namespace std::literals;

//namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"

#include "Config.h"

#define BETTER_SAFE_THAN_SORRY