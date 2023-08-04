//
//  IdSelector.h
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef IdSelector_hpp
#define IdSelector_hpp

#include "Selector.h"

#include <stdio.h>
#include <iostream>

namespace future {
    class IdSelector: public Selector {
    public:
        IdSelector(const std::string& id)
        {
            m_id = id;
            m_selectorType = Selector::IDSelector;
        };

        ~IdSelector() {}

        inline std::string getIdIdentifier() {
            return m_id;
        }

        bool isBaseSelector();
        int weight();
        virtual std::string description();
    private:
        std::string m_id;
    };
}

#endif /* IdSelector_hpp */
