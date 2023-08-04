//
//  Selector.h
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef Selector_hpp
#define Selector_hpp

#include "utils/ContainerUtil.h"
#include "utils/StringUtil.h"

#include <stdio.h>
#include <iostream>
#include <map>

namespace future {
class Selector {
	friend class CombineSelector;
public:
	enum SelectorType {
		TypeSelector,
		IDSelector,
		ClassSelector,
		UniversalSelector,
		AttributeSelector,
		PseudoSelector,
		SimpleSelectorSequence,
		CombineSelector,
		SelectorGroup,
		SignSelector
	};
public:
	Selector();
	virtual ~Selector();

	inline std::string& getRuleData() {
		return m_ruleData;
	}

	void setRuleData(const std::string& data) {
		m_ruleData = data;
		m_ruleDataMap.clear();
	}

	std::map<std::string, std::string> getRuleDataMap();

	SelectorType getType() {
		return m_selectorType;
	}

	void setHostCSSFilePath(const std::string& path) {
		m_hostCSSFilePath = path;
	}

	std::string getHostCSSFilePath() {
		return m_hostCSSFilePath;
	}

	virtual bool isBaseSelector() = 0;
	virtual int weight() = 0;

	virtual std::string description();
protected:
	std::string m_hostCSSFilePath;
    std::string m_ruleData;
	SelectorType m_selectorType;

	std::map<std::string, std::string> m_ruleDataMap;
};
}

#endif /* Selector_hpp */
