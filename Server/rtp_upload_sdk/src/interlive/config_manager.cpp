#include "config_manager.h"
#include <assert.h>
#include "util/log.h"
#include <algorithm>

#ifndef nullptr
#define nullptr NULL
#endif

using namespace std;

static ConfigManager g_config_manager;

ConfigManager& ConfigManager::get_inst()
{
    return g_config_manager;
}

ConfigManager::ConfigManager() : 
    _name_config_modules(), _order_config_modules()
{
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::set_default_config()
{
    for (cm_vec_t::iterator iter = _order_config_modules.begin(); 
        iter != _order_config_modules.end(); ++iter)
    {
        ConfigModule* cm = *iter;
        ASSERTV(cm != nullptr);

        cm->set_default_config();
    }
}

bool ConfigManager::load_config(xmlnode* xml_config)
{
    for (cm_vec_t::iterator iter = _order_config_modules.begin();
        iter != _order_config_modules.end(); ++iter)
    {
        ConfigModule* cm = *iter;
        ASSERTR(cm != nullptr, false);

        if (!(cm->load_config(xml_config)))
            return false;
    }

    return true;
}

bool ConfigManager::load_config(const char* name, xmlnode* xml_config)
{
    ASSERTR(name != nullptr, false);

    cm_map_t::iterator iter = _name_config_modules.find(string(name));
    if (iter == _name_config_modules.end())
        return false;

    return iter->second->load_config(xml_config);
}

bool ConfigManager::reload() const
{
    for (cm_vec_t::const_iterator iter = _order_config_modules.begin();
        iter != _order_config_modules.end(); ++iter)
    {
        ConfigModule* cm = *iter;
        ASSERTR(cm != nullptr, false);

        if (!(cm->reload()))
            return false;
    }

    return true;
}

bool ConfigManager::reload(const char* name) const
{
    ASSERTR(name != nullptr, false);

    cm_map_t::const_iterator iter = _name_config_modules.find(string(name));
    if (iter == _name_config_modules.end())
        return false;

    return iter->second->reload();
}

bool ConfigManager::reload(ConfigManager& new_config_manager)
{
    g_config_manager._name_config_modules.swap(new_config_manager._name_config_modules);
    g_config_manager._order_config_modules.swap(new_config_manager._order_config_modules);

    return reload();
}

ConfigModule* ConfigManager::get_config_module(const char* name)
{
    ASSERTR(name != nullptr, false);

    cm_map_t::iterator iter = _name_config_modules.find(string(name));
    if (iter == _name_config_modules.end())
        return nullptr;
    else
        return iter->second;
}

ConfigModule* ConfigManager::set_config_module(ConfigModule* config_module)
{
    // mt unsafe!

    ASSERTR(config_module != nullptr, false);
    ASSERTR(config_module->module_name() != nullptr, false);

    string module_name(config_module->module_name());

    cm_map_t::iterator iter_name_map = _name_config_modules.find(module_name);
    if (iter_name_map == _name_config_modules.end())
    {
        _name_config_modules[module_name] = config_module;
        _order_config_modules.push_back(config_module);
        return nullptr;
    }
    else
    {
        ConfigModule* old_cm = iter_name_map->second;

        cm_vec_t::iterator iter_order_vec = std::find(_order_config_modules.begin(), _order_config_modules.end(), old_cm);
        ASSERTR(iter_order_vec != _order_config_modules.end(), false);

        *iter_order_vec = config_module;
        iter_name_map->second = config_module;

        return old_cm;
    }
}

void ConfigManager::foreach_config_module(config_module_iter_func iter_func)
{
    for (cm_vec_t::iterator iter = _order_config_modules.begin();
        iter != _order_config_modules.end(); ++iter)
    {
        ConfigModule* cm = *iter;
        ASSERTR(cm != nullptr, false);

        if (!(iter_func(cm)))
            break;
    }
}

void ConfigManager::dump_config() const
{
    for (cm_vec_t::const_iterator iter = _order_config_modules.begin();
        iter != _order_config_modules.end(); ++iter)
    {
        ConfigModule* cm = *iter;
        ASSERTV(cm != nullptr);

        cm->dump_config();
    }
}
