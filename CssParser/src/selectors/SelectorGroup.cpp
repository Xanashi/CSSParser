//
//  SelectorGroup.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "selectors/SelectorGroup.h"

namespace future {
    
    void GroupSelector::addSelector(future::Selector *s)
    {
        if (!s) {
            return ;
        }
        m_selectors.push_back(s);
    }
    
    GroupSelector::~GroupSelector()
    {
        CleanContainer<Selector *>(m_selectors);
    }

    bool GroupSelector::isBaseSelector()
    {
    	return false;
    }

    int GroupSelector::weight()
    {
        if (m_selectors.size() < targetSelector + 1) {
            return 0;
        }
    	return m_selectors[targetSelector]->weight();
    }

    void GroupSelector::setTargetSelector(Selector* s) {
        targetSelector = (find(m_selectors.begin(), m_selectors.end(), s) - m_selectors.begin());
    }
    
    std::string GroupSelector::description()
    {
        std::string desc = "Group Selector:\n[";
        for (const auto& s : m_selectors) {
            desc += "\n    " + s->description() + ",";
        }
        desc.pop_back();
        desc += "\n]";
        return desc;
    }
}
