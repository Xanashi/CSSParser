//
//  SelectorSequence.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "selectors/SelectorSequence.h"

namespace future {
    void SequenceSelector::appendSelector(Selector *s)
    {
        if (!s) {
            return;
        }
        m_selectors.push_back(s);
    }
    
    SequenceSelector::~SequenceSelector()
    {
        CleanContainer<Selector *>(m_selectors);
    }

    bool SequenceSelector::isBaseSelector()
    {
    	return false;
    }

    int SequenceSelector::weight()
    {
    	std::list<Selector *>::iterator it = m_selectors.begin();
    	std::list<Selector *>::iterator end  = m_selectors.end();
    	int w = 0;
    	while(it != end) {
    		w += (*it++)->weight();
    	}
    	return w;
    }
    
    std::string SequenceSelector::description()
    {
        std::string desc = "Sequence Selector:\n{";
        auto it = m_selectors.begin();
        auto end = m_selectors.end();
        while (it != end) {
            desc += "\n    " +(*it)->description();
            ++it;
        }
        desc += "\n}";
        return desc;
    }
}
