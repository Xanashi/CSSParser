
#ifndef NODEPSEUDOSELECTORRESOLVER_H_
#define NODEPSEUDOSELECTORRESOLVER_H_

#include <map>
#include <stdio.h>

#include "selectors/PseudoSelector.h"

class INodeSelector;

namespace future {
    class PseudoSelector;

    struct TraverseUtilStruct {
        INodeSelector* selfNode;
        int idx;
        int totalCount;

        TraverseUtilStruct(INodeSelector* n)
        {
            selfNode = n;
            idx = 0;
            totalCount = 0;
        }
    };

    class NodePseudoSelectorResolver {
    public:
        static bool DoesNodeMatchPseudo(INodeSelector*, PseudoSelector*);

    private:
        typedef void(*traverseAction)(INodeSelector* node, bool& stop, TraverseUtilStruct* baseInfo);

        static std::map<std::string, bool>& GetDynamicPseudoClassMap();
        
        static bool TraverseElementNodeSiblings(INodeSelector*node, traverseAction ac, TraverseUtilStruct* baseInfo);
        
        static int  IndexOfSiblings(INodeSelector* node);
        static int  IndexEqualTypeOfSiblings(INodeSelector* node);        
        static int  LastIndexOfSiblings(INodeSelector* node);
        static int  LastIndexEqualTypeOfSiblings(INodeSelector* node);
        
        static bool IndexMatchesParameter(future::PseudoSelector* selector, int idx);        
        static bool IndexMatchesPolynomial(PseudoSelector::Parameter* parameter, int idx);
    };
}

#endif /* NODEPSEUDOSELECTORRESOLVER_H_ */
