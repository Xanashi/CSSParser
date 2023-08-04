#pragma once

#include <string>
#include <vector>

class INodeSelector {
public:	
	virtual ~INodeSelector() {}

	virtual std::string GetTag() = 0;
	virtual std::string GetId() = 0;
	virtual std::vector<std::string> GetClasses() = 0;
	virtual std::pair<std::string, std::string> GetAttribute(std::string key) = 0;

	virtual INodeSelector* GetParent() = 0;
	virtual std::vector<INodeSelector*> GetChildren() = 0;
	virtual int GetIndexWithinParent() = 0;
};