#pragma once

#include "utils/StringUtil.h"

#include <string>
#include <map>

namespace future {

class CSSUtil
{
public:
	static std::map<std::string, std::string> GetRuleMapFromString(std::string ruleData) {
		std::map<std::string, std::string> map;
		
		if (!ruleData.empty()) {
			StringUtil::trim(ruleData);
			StringUtil::deletechar(ruleData, '\n');
			auto keyValues = StringUtil::splitButSkipBrackets(ruleData, ';');

			for (const auto& pair : keyValues) {
				auto keyAndValue = StringUtil::splitButSkipBrackets(pair, ':');
				if (keyAndValue.size() < 2) {
					continue;
				}
				map[StringUtil::trim(keyAndValue[0])] = StringUtil::trim(keyAndValue[1]);
			}
		}

		return map;
	}
};

}