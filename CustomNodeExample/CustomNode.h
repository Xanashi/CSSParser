#pragma once

#include <resolvers/INodeSelector.h>
#include "utils/StringUtil.h"
#include "StyleMap.h"

#include <map>

class CustomNode : public INodeSelector
{
public:
    CustomNode(std::string tag, std::string id, std::string classes,
        std::map<std::string, std::string> attributes, std::string inlineStyle,
        std::vector<CustomNode*> children)
        : tag(tag), id(id), classes(future::StringUtil::split(classes, ' ')), 
          attributes(attributes), inlineStyle(inlineStyle)
    {
        int idx = 0;
        for (const auto& child : children)
        {
            child->SetParent(this, idx++);
            this->children.push_back(child);
        }
    }

    virtual ~CustomNode()
    {
        for (const auto& child : children)
            delete child;
    }

    void SetParent(CustomNode* parent, int indexWithinParent)
    {
        this->parent = parent;
        this->indexWithinParent = indexWithinParent;
    }

    std::string GetIdDesc() { return !id.empty() ? ("#" + id) : ""; }
    std::string GetClassesDesc() { return !classes.empty() ? ("(" + future::StringUtil::join(classes, ' ') + ")") : ""; }

    // INodeSelector Implementation

    virtual std::string GetTag() override
    {
        return tag;
    }

    virtual std::string GetId() override
    {
        return id;
    }

    virtual std::vector<std::string> GetClasses() override
    {
        return classes;
    }

    virtual std::pair<std::string, std::string> GetAttribute(std::string key) override
    {
        std::string value = attributes.find(key) != attributes.end() ? attributes[key] : std::string();
        return std::make_pair(key, value);
    }

    virtual INodeSelector* GetParent() override
    {
        return parent;
    }

    virtual std::vector<INodeSelector*> GetChildren() override
    {
        std::vector<INodeSelector*> convertedChildren(children.begin(), children.end());
        return convertedChildren;
    }

    virtual int GetIndexWithinParent() override
    {
        return indexWithinParent;
    }

    // Node Members

    std::string tag;
    std::string id;
    std::vector<std::string> classes;
    std::map<std::string, std::string> attributes;
    std::string inlineStyle;

    CustomNode* parent = nullptr;
    std::vector<CustomNode*> children;

    int indexWithinParent = 0;

    StyleMap styles;
};
