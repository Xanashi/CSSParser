//
//  TypeSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "selectors/TypeSelector.h"

namespace future {

	bool TypeSelector::isBaseSelector()
	{
		return true;
	}

	int TypeSelector::weight()
	{
		return 1;
	}
    
    std::string TypeSelector::description()
    {
		return "Type Selector: '" + m_typeName + "'";
    }
}
