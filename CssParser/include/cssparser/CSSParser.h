//
//  CSSParser.h
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/7.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#ifndef CSSParser_hpp
#define CSSParser_hpp

#include "CSSLex.h"
#include "CSSParserStatus.h"
#include "KeywordItem.h"
#include "selectors/SelectorsHeader.h"
#include "selectors/PseudoSelector.h"

#include <stdio.h>
#include <iostream>
#include <stack>
#include <vector>

namespace future {

    class Lex;
    class Selector;

    class CSSParser 
    {
    public:
        struct ASTNode 
        {
            Selector* head;
            ASTNode* left;
            ASTNode* right;
            ASTNode()
            {
                head = NULL;
                left = NULL;
                right = NULL;
            }
        };
    public:
        CSSParser();
        ~CSSParser();

        bool parseByFile(const std::string& cssFile);
        bool parseByString (const std::string& cssString);
        
        std::vector<Selector *>    getSelectors();
        std::vector<KeywordItem *> getKeywords();

        void cleanRes();

    private:
        typedef void(*treeTranverseAction)(ASTNode *);
        typedef CSSParser::ASTNode *(*treeTranverseWithUserDataAction)(std::stack<CSSParser::ASTNode *>* stack);
        friend CSSParser::ASTNode* TreeTranverseCreateExpressionAction(std::stack<CSSParser::ASTNode *>*);

        static void             initialASTNode(ASTNode *target, Selector* head, ASTNode* left, ASTNode* right);
        static void             pushOperatedElement(std::stack<ASTNode *>&, Selector* head);

        bool                    parse();
        void                    prepareByFile(const std::string& filePath);
        void                    prepareByString(const std::string& cssString);
        void                    clean();

        bool                    startSelector(CSSTokenType);
        bool                    tokenHasInfo(CSSTokenType);
        bool                    topHaveSign(std::stack<Selector *>&);
        Selector*               getSelector(Lex::CSSToken* token);
        PseudoSelector::Parameter* getFunctionParamenter();
        std::list<ASTNode *>    createATS(std::stack<Selector *>&);
        void                    pushSign(std::stack<Selector *>&, SignSelector::SignType);
        void                    buildReversePolishNotation(std::stack<ASTNode*>& operatorStack, std::stack<ASTNode*>& operandStack);

        void                    RMLtranverseAST(ASTNode *root, treeTranverseAction action);
        void                    LRMtranverseAST(ASTNode *root, treeTranverseAction action);
        void                    LMRtranverseAST(ASTNode *root, treeTranverseAction action);
        void                    MLRtranverseAST(ASTNode *root, treeTranverseWithUserDataAction action, void *userData);

    private:
        Lex*                        m_lexer;
        CSSParserStatus             m_status;
        std::string                 m_hostCssFile;
        std::vector<Selector *>     m_selectors;
        std::vector<KeywordItem *>  m_keywords;
        std::list<Selector *> 		m_signSelecors;
    };
}

#endif /* CSSParser_hpp */
