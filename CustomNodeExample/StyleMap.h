#pragma once

#include <string>
#include <map>

class StyleMap
{
public:
    bool Contains(std::string key)
    {
        return styleMap.find(key) != styleMap.end();
    }

    void AddOrOverwrite(std::string key, std::string value, int specificity)
    {
        styleMap[key] = std::make_pair(value, specificity);
    }

    std::string GetValue(std::string key)
    {
        return styleMap[key].first;
    }

    int GetSpecificity(std::string key)
    {
        return styleMap[key].second;
    }

private:
    std::map<std::string, std::pair<std::string, int>> styleMap;
};