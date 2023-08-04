
#include "resolvers/NodeSelectorResolver.h"
#include "resolvers/NodePseudoSelectorResolver.h"
#include "resolvers/INodeSelector.h"
#include "utils/StringUtil.h"

namespace future {
        
    bool NodeSelectorResolver::DoesNodeMatchSelector(INodeSelector* node, Selector* selector)
    {
        switch(selector->getType()) {
            case Selector::TypeSelector: {
                return DoesNodeMatchTypeSelector(node, dynamic_cast<TypeSelector *>(selector));
            }
            case Selector::IDSelector: {
                return DoesNodeMatchIDSelector(node, dynamic_cast<IdSelector *>(selector));
            }
            case Selector::ClassSelector: {
                return DoesNodeMatchClassSelector(node, dynamic_cast<ClassSelector *>(selector));
            }
            case Selector::UniversalSelector: {
                return true;
            }
            case Selector::AttributeSelector: {
                return DoesNodeMatchAttributeSelector(node, dynamic_cast<AttributeSelector *>(selector));
            }
            case Selector::PseudoSelector: {
                return DoesNodeMatchPseudoSelector(node, dynamic_cast<PseudoSelector *>(selector));
            }
            case Selector::SimpleSelectorSequence: {
                return DoesNodeMatchSequenceSelector(node, dynamic_cast<SequenceSelector *>(selector));
            }
            case Selector::CombineSelector: {
                return DoesNodeMatchCombineSelector(node, dynamic_cast<CombineSelector *>(selector));
            }
            case Selector::SelectorGroup: {
                return DoesNodeMatchGroupSelector(node, dynamic_cast<GroupSelector *>(selector));
            }
            default:
                return false;
        }
    }
    
    bool NodeSelectorResolver::DoesNodeMatchClassSelector(INodeSelector* node, ClassSelector* selector)
    {
        auto classes = node->GetClasses();
        return std::find(classes.begin(), classes.end(), selector->getClassIdentifier()) != classes.end();
    }
    
    bool NodeSelectorResolver::DoesNodeMatchIDSelector(INodeSelector* node, IdSelector* selector)
    {
        return node->GetId() == selector->getIdIdentifier();
    }
    
    bool NodeSelectorResolver::DoesNodeMatchTypeSelector(INodeSelector* node, TypeSelector* selector)
    {
        auto tag = StringUtil::tolower(node->GetTag());
        if (tag.empty()) {
            return false;
        }

        std::string selectorTagName = StringUtil::tolower(selector->getTagName());
        return selectorTagName == tag;
    }
    
    bool NodeSelectorResolver::DoesNodeMatchAttributeSelector(INodeSelector* node, AttributeSelector* selector)
    {
        std::string key = selector->getKey();
        std::string value = StringUtil::tolower(selector->getValue());
        AttributeSelector::attributeFilterRule kvRule = selector->getKVRule();
        
        if (key.empty()) {
            return false;
        }

        auto attr = node->GetAttribute(key);
        if (attr.first.empty()) {
            return false;
        }

        std::string nodeAttrValue = StringUtil::tolower(attr.second);

        bool ret = false;
        switch (kvRule) {
            case AttributeSelector::Equal: {
                return nodeAttrValue == value;
            }
            case AttributeSelector::DashMatch: {
                if (nodeAttrValue.find("-") == std::string::npos) {
                    break;
                }
                auto attrs = StringUtil::split(nodeAttrValue, '-');
                return *attrs.begin() == value;
            }
            case AttributeSelector::Prefix: {
                return nodeAttrValue.find(value, 0) == 0;
            }
            case AttributeSelector::Suffix: {
                return nodeAttrValue.rfind(value) + value.length() == nodeAttrValue.length();
            }
            case AttributeSelector::Include: {
                if (nodeAttrValue.find(" ") == std::string::npos) {
                    break;
                }
                auto attrs = StringUtil::split(nodeAttrValue, ' ');
                for (const auto& attr : attrs) {
                    if (attr == value) {
                        return true;
                    }
                }
                break;
            }
            case AttributeSelector::Substring: {
                return nodeAttrValue.find(value, 0) != std::string::npos;
            }
            case AttributeSelector::NoRule: {
                return true;
            }
            default:
                return false;
        }
        return ret;
    }
    
    bool NodeSelectorResolver::DoesNodeMatchPseudoSelector(INodeSelector *node, PseudoSelector* selector)
    {
        return NodePseudoSelectorResolver::DoesNodeMatchPseudo(node, selector);
    }
    
    bool NodeSelectorResolver::DoesNodeMatchSequenceSelector(INodeSelector *node, SequenceSelector* selector)
    {
        std::list<Selector *>ss = selector->getContrains();
        for(const auto& s : ss) {
            if (!DoesNodeMatchSelector(node, s)) { // all selectors must return true
                return false;
            }
        }
        return true;       
    }
    
    bool NodeSelectorResolver::DoesNodeMatchGroupSelector(INodeSelector *node, GroupSelector* selector)
    {
        std::vector<Selector *>ss = selector->getAllSelectors();
        for (const auto& s : ss) {
            if (DoesNodeMatchSelector(node, s)) { // any selectors can return true
                selector->setTargetSelector(s);
                return true;
            }
        }
        return false;
    }
    
    bool NodeSelectorResolver::DoesNodeMatchCombineSelector(INodeSelector *node, CombineSelector* selector)
    {
        selector->matchingNodes.clear();
        Selector* before = selector->getBefore();
        Selector* after = selector->getAfter();

        if (!before || !after) {
            return false;
        }

        INodeSelector* parent = node->GetParent();

        if (!parent || !DoesNodeMatchSelector(node, after)) {
            return false;
        }

        std::vector<INodeSelector*> nodesToCheck;
        if (after->getType() == Selector::CombineSelector) {
            auto cs = dynamic_cast<CombineSelector*>(after);
            nodesToCheck.insert(nodesToCheck.end(), cs->matchingNodes.begin(), cs->matchingNodes.end());
        }
        else {
            nodesToCheck.push_back(node);
        }

        for (size_t i = 0; i < nodesToCheck.size(); i++) {
            node = nodesToCheck[i];
            parent = node->GetParent();

            if (!parent) { continue; }

            switch (selector->getCombineType()) 
            {
                case CombineSelector::NormalSibling: {
                    int idx = node->GetIndexWithinParent();
                    auto children = parent->GetChildren();
                    for (const auto& sibling : children) {
                        if (sibling->GetIndexWithinParent() >= idx) {
                            continue;
                        }
                        if (DoesNodeMatchSelector(sibling, before)) {
                            selector->matchingNodes.push_back(sibling);
                        }
                    }
                    break;
                }
                case CombineSelector::InstanceSibling: {
                    auto children = parent->GetChildren();
                    INodeSelector* lastElementNode = nullptr;
                    for (const auto& sibling : children) {
                        if (lastElementNode && sibling == node) {
                            if (DoesNodeMatchSelector(lastElementNode, before)) {
                                selector->matchingNodes.push_back(lastElementNode);
                            }
                        }
                        lastElementNode = sibling;                    
                    }
                    break;
                }
                case CombineSelector::InstanceInherical: {
                    if (DoesNodeMatchSelector(parent, before)) {
                        selector->matchingNodes.push_back(parent);
                    }
                    break;
                }
                case CombineSelector::NormalInherical: {
                    while(parent) {
                        if (DoesNodeMatchSelector(parent, before)) {
                            selector->matchingNodes.push_back(parent);
                            break;
                        }
                        parent = parent->GetParent();
                    }
                    break;
                }
                default: break;
            }
        }

        return !selector->matchingNodes.empty();
    }    
}
