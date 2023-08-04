//
//  SelectorGroup.h
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef SelectorGroup_hpp
#define SelectorGroup_hpp

#include "Selector.h"

#include <stdio.h>
#include <iostream>
#include <list>

namespace future {
    class GroupSelector: public Selector {
    public:
        GroupSelector()
        {
            m_selectorType = Selector::SelectorGroup;
        }

        ~GroupSelector();
        
        void addSelector(Selector *);
        
        std::vector<Selector *>getAllSelectors()
        {
            return m_selectors;
        }

        bool isBaseSelector();
        int weight();
        void setTargetSelector(Selector* s);
        virtual std::string description();

    private:
        std::vector<Selector *> m_selectors;
        unsigned int targetSelector = 0;
    };
}

#endif /* SelectorGroup_hpp */
