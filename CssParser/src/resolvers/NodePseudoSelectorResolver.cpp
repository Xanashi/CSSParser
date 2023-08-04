
#include "resolvers/NodePseudoSelectorResolver.h"
#include "resolvers/INodeSelector.h"
#include "utils/StringUtil.h"

#include <string>

namespace future {  
    static std::map<std::string, bool> dynamicPsudoClassCacheMap;

    static void NodeIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* userData);
    static void NodeFullIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* userData);
    static void NodeTypeEqualIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* userData);
    static void NodeFullTypeEqualIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* userData);

    std::map<std::string, bool>& NodePseudoSelectorResolver::GetDynamicPseudoClassMap()
    {
        if (!dynamicPsudoClassCacheMap.empty()) {
            return dynamicPsudoClassCacheMap;
        }

        dynamicPsudoClassCacheMap["link"]          = true;
        dynamicPsudoClassCacheMap["visited"]       = true;
        dynamicPsudoClassCacheMap["hover"]         = true;
        dynamicPsudoClassCacheMap["active"]        = true;
        dynamicPsudoClassCacheMap["focus"]         = true;
        dynamicPsudoClassCacheMap["enable"]        = true;
        dynamicPsudoClassCacheMap["disable"]       = true;
        dynamicPsudoClassCacheMap["checked"]       = true;
        dynamicPsudoClassCacheMap["indeterminate"] = true;
        dynamicPsudoClassCacheMap["target"]        = true;

        return dynamicPsudoClassCacheMap;
    }
    
    bool NodePseudoSelectorResolver::DoesNodeMatchPseudo(INodeSelector *node, future::PseudoSelector *selector)
    {
        if (!node || !selector) {
            return false;
        }
        std::string ruleData = selector->getPseudoData();
        if (ruleData.empty()) {
            return false;
        }
        if (GetDynamicPseudoClassMap()[ruleData]) {
            return true;
        }

        switch (strhash(ruleData.c_str())) 
        {
        case strhash("root"):
            return !node->GetParent();

        case strhash("nth-child"):
            return IndexMatchesParameter(selector, IndexOfSiblings(node));

        case strhash("nth-last-child"):
            return IndexMatchesParameter(selector, LastIndexOfSiblings(node));

        case strhash("nth-of-type"):
            return IndexMatchesParameter(selector, IndexEqualTypeOfSiblings(node));

        case strhash("nth-last-of-type"):
            return IndexMatchesParameter(selector, LastIndexEqualTypeOfSiblings(node));

        case strhash("first-child"):
            return IndexOfSiblings(node) == 1;

        case strhash("last-child"):
            return LastIndexOfSiblings(node) == 1;

        case strhash("first-of-type"):
            return IndexEqualTypeOfSiblings(node) == 1;

        case strhash("last-of-type"):
            return LastIndexEqualTypeOfSiblings(node) == 1;

        case strhash("only-child"):
            return IndexOfSiblings(node) == 1 && LastIndexOfSiblings(node) == 1;

        case strhash("only-of-type"):
            return IndexEqualTypeOfSiblings(node) == 1 && LastIndexEqualTypeOfSiblings(node) == 1;

        case strhash("empty"):
            return !node->GetChildren().size();

        case strhash("lang"): {
            auto langAttr = node->GetAttribute("lang");
            PseudoSelector::Parameter* p = selector->getParameter();
            return (p && !langAttr.first.empty()) && (p->pString == langAttr.second);
        }
        }

        return true;
    }
    

    // private functions
    
    int NodePseudoSelectorResolver::IndexOfSiblings(INodeSelector* node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeIndexCounter, &baseInfo);
        return ret ? baseInfo.idx : 0;
    }
    
    int NodePseudoSelectorResolver::IndexEqualTypeOfSiblings(INodeSelector *node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeTypeEqualIndexCounter, &baseInfo);
        return ret ? baseInfo.idx : 0;
    }
    
    int NodePseudoSelectorResolver::LastIndexOfSiblings(INodeSelector *node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeFullIndexCounter, &baseInfo);
        return ret ? baseInfo.totalCount - baseInfo.idx + 1 : 0;
    }
    
    int NodePseudoSelectorResolver::LastIndexEqualTypeOfSiblings(INodeSelector *node)
    {
        TraverseUtilStruct baseInfo(node);
        bool ret = TraverseElementNodeSiblings(node, NodeFullTypeEqualIndexCounter, &baseInfo);
        return ret ? baseInfo.totalCount - baseInfo.idx + 1 : 0;
    }
    
    bool NodePseudoSelectorResolver::TraverseElementNodeSiblings(INodeSelector *node, traverseAction ac, TraverseUtilStruct* baseInfo)
    {
        INodeSelector* parent = node->GetParent();
        if (!parent) {
            return false;
        }
        bool stop = false;
        auto children = parent->GetChildren();
        for (const auto& sibling : children) {
            ac(sibling, stop, baseInfo);
            if (stop) {
                break;
            }
        }
        return true;
    }
    
    bool NodePseudoSelectorResolver::IndexMatchesParameter(future::PseudoSelector* selector, int idx)
    {
        auto parameter = selector->getParameter();
        if (!parameter) {
            return false;
        }

        bool match = false;
        switch (parameter->type) {
            case PseudoSelector::ParameterType::NUMBER: {
                match = idx == parameter->pNumber;
                break;
            }
            case PseudoSelector::ParameterType::STRING: {
                match = false;
                break;
            }
            case PseudoSelector::ParameterType::POLYNOMIAL: {
                match = IndexMatchesPolynomial(parameter, idx);
                break;
            }
            case PseudoSelector::ParameterType::IDENT: {
                PseudoSelector::Parameter p;
                p.polynomial.coefficient = 2;
                if (StringUtil::tolower(parameter->pString) == "even") {
                    p.polynomial.constant = 0;
                    match = IndexMatchesPolynomial(&p, idx);
                } else if (StringUtil::tolower(parameter->pString) == "odd") {
                    p.polynomial.constant = 1;
                    match = IndexMatchesPolynomial(&p, idx);
                } else {
                    match = false;
                }
                break;
            }
                
            default:
                break;
        }
        return match;
    }
    
    bool NodePseudoSelectorResolver::IndexMatchesPolynomial(PseudoSelector::Parameter *parameter, int idx)
    {
        if (idx < parameter->polynomial.constant) {
            return false;
        }
        return !((idx - parameter->polynomial.sign * parameter->polynomial.constant) % parameter->polynomial.coefficient);
    }   
    

    // callbacks

    void NodeIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        baseInfo->idx += 1;
        if (node == baseInfo->selfNode) {
            stop = true;
        }
    }
    
    void NodeFullIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        baseInfo->totalCount += 1;
        if (node == baseInfo->selfNode) {
            baseInfo->idx = baseInfo->totalCount;
        }
    }
    
    void NodeTypeEqualIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        if (node->GetTag() == baseInfo->selfNode->GetTag()) {
            baseInfo->idx += 1;
        }
        if (node == baseInfo->selfNode) {
            stop = true;
        }
    }
    
    void NodeFullTypeEqualIndexCounter(INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo)
    {
        if (node->GetTag() == baseInfo->selfNode->GetTag()) {
            baseInfo->totalCount += 1;
        }
        if (node == baseInfo->selfNode) {
            baseInfo->idx = baseInfo->totalCount;
        }
    }
}
