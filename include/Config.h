#pragma once

#include "toml.h"

class Config
{
private:
    Config();
    Config(const Config&) = delete;
    Config(Config&&) = delete;

public:
    ~Config() { CloseTable(); }

    static Config& Instance();

    const char* GetPluginFileName() const { return pluginFileName.data(); }
    template <typename T>
    T Get(const std::string_view& section, const std::string_view& key, T defaultValue)
    {
        return GetTable()[section][key].value_or(defaultValue);
    }
    template <typename T>
    void Set(const std::string_view& section, const std::string_view& key, T value)
    {
        GetSectionTable(section).insert_or_assign(key, value);
        modified = true;
    }
    template<>
    void Set<RE::FormID>(const std::string_view& section, const std::string_view& key, RE::FormID value)
    {
        GetSectionTable(section).insert_or_assign(section, toml::table{ { key, value } }, toml::value_flags::format_as_hexadecimal);
        modified = true;
    }
    toml::table& GetTable();
    toml::table& GetSectionTable(const std::string_view& section);
    void CloseTable();

private:
    toml::table* table{ nullptr };
    bool modified{ false };
    std::string_view pluginFileName;
};