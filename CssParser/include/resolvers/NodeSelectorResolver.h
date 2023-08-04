
#ifndef NODESELECTORRESOLVER_H_
#define NODESELECTORRESOLVER_H_

#include "selectors/SelectorsHeader.h"

class INodeSelector;

namespace future {

class NodeSelectorResolver {
public:
	NodeSelectorResolver() {}
	virtual ~NodeSelectorResolver() {}
    /**
     * Check Whether a selector can apply to the node
     */
	static bool DoesNodeMatchSelector(INodeSelector* node, Selector* selector);

private:
	static bool DoesNodeMatchIDSelector(INodeSelector* node, IdSelector* selector);
	static bool DoesNodeMatchClassSelector(INodeSelector* node, ClassSelector* selector);
	static bool DoesNodeMatchTypeSelector(INodeSelector* node, TypeSelector* selector);
	static bool DoesNodeMatchAttributeSelector(INodeSelector* node, AttributeSelector* selector);
	static bool DoesNodeMatchPseudoSelector(INodeSelector* node, PseudoSelector* selector);
	static bool DoesNodeMatchSequenceSelector(INodeSelector* node, SequenceSelector* selector);
	static bool DoesNodeMatchCombineSelector(INodeSelector* node, CombineSelector* selector);
	static bool DoesNodeMatchGroupSelector(INodeSelector* node, GroupSelector* selector);
};

}

#endif /* NODESELECTORRESOLVER_H_ */
