#include "Config.h"

Config::Config()
{
    pluginFileName = Get("General", "PluginFileName", Plugin::PLUGINFILE);
}

Config& Config::Instance()
{
    static Config instance;
    return instance;
}

toml::table& Config::GetTable()
{
    if (table == nullptr)
    {
        try
        {
            auto parsedTable{ toml::parse_file(Plugin::CONFIGFILE) };
            table = new toml::table(std::move(parsedTable));
        }
        catch (const toml::parse_error&)
        {
            // Just create an empty table and save it later if there are any changes.
            table = new toml::table();
        }
    }
    return *table;
}
toml::table& Config::GetSectionTable(const std::string_view& section)
{
    GetTable().insert(section, toml::table{});
    return *GetTable().get_as<toml::table>(section);
}
void Config::CloseTable()
{
    if (table)
    {
        if (modified)
        {
            std::ofstream file{ Plugin::CONFIGFILE.data()};
            if (file.is_open())
            {
                file << *table;
                file.close();
            }
            else
            {
                SKSE::log::error("Failed to open config file at '{}' for writing.", Plugin::CONFIGFILE);
            }
        }
        delete table;
        table = nullptr;
    }
}