//
//  IdSelector.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include "selectors/IdSelector.h"

namespace future {

	bool IdSelector::isBaseSelector()
	{
		return true;
	}

	int IdSelector::weight()
	{
		return 100;
	}
    
    std::string IdSelector::description()
    {
        return "Id Selector: '#" + m_id + "'";
    }
}
