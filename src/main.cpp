#include "Hooks.h"
#include "Rings.h"
#include "Papyrus.h"

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto vssink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#endif

		auto path = SKSE::log::log_directory();
		if (!path)
		{
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

#ifndef NDEBUG
		spdlog::logger multilog("multi_sink", { sink, vssink });

		auto log = std::make_shared<spdlog::logger>(std::move(multilog));

		const auto level = spdlog::level::trace;
		log->set_level(level);
		log->flush_on(level);
#else
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

		const auto level = spdlog::level::info;
		log->set_level(level);
		log->flush_on(level);
#endif

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
{
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName(Plugin::AUTHOR);
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	//v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

void MessagingCallback(SKSE::MessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:
		{
			RE::TESDataHandler* data = RE::TESDataHandler::GetSingleton();

			Rings::GetSingleton()->UpdateData(data);

			Config::Instance().CloseTable();
		}
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * skse)
{
	InitializeLog();
	std::string version = Plugin::VERSION.string();
	SKSE::log::info("{} v{}"sv, Plugin::NAME, version);

	SKSE::Init(skse);

	const SKSE::MessagingInterface* messaging = SKSE::GetMessagingInterface();
	if (!messaging)
	{
		SKSE::log::error("Unable to get messaging interface. Aborting plugin load.");
		return false;
	}
	messaging->RegisterListener(MessagingCallback);

	const SKSE::PapyrusInterface* papyrus = SKSE::GetPapyrusInterface();
	if (!papyrus)
	{
		SKSE::log::error("Unable to get papyrus interface. Aborting plugin load.");
		return false;
	}
	if (!papyrus->Register(Papyrus::RegisterFunctions))
	{
		SKSE::log::error("Failed to register papyrus functions. Aborting plugin load.");
		return false;
	}

	SKSE::AllocTrampoline(2048);

	Hooks::Init();

	return true;
}
