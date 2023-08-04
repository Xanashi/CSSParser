//
//  main.cpp
//  DDCSSParser
//
//  Created by 1m0nster on 2018/8/1.
//  Copyright © 2018 1m0nster. All rights reserved.
//

#include <iostream>
#include <string.h>

#include "cssparser/CSSParser.h"
#include "cssparser/KeywordItem.h"
#include "gumbo/gumbo.h"
#include "gumbo/GumboInterface.h"
#include "applicability/HTMLCSSRefAdaptor.h"


void Applicability(GumboNode* root, future::Selector* selector);
void AllNodesApplyToSelector(GumboNode* root, future::Selector* selector, std::list<GumboNode*>& list);

int main(int argc, const char* argv[]) {
    if (argc < 3) {
        printf("Please enter html path and css path\n");
        return 0;
    }
    const char* htmlPath = argv[1];
    const char* cssPath = argv[2];
    if (!strlen(htmlPath)) {
        printf("HTML path is empty\n");
        return 0;
    }
    if (!strlen(cssPath)) {
        printf("css path is empty\n");
        return 0;
    }

    FILE* f;
    fopen_s(&f, htmlPath, "r");
    if (!f) { printf("HTML file open failed\n"); return 0; }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    if (size == 0) { printf("HTML file is empty\n"); return 0; }
    fseek(f, 0, SEEK_SET);
    char* ptr = new char[size + 1];
    memset(ptr, 0, size + 1);
    fread((void*)ptr, size, 1, f);
    fclose(f);

    future::GumboInterface gi(ptr, "2");
    future::CSSParser* parser = new future::CSSParser;
    parser->parseByFile(cssPath);
    std::vector<future::Selector*>selectors = parser->getSelectors();

    printf("********** All Keywords **********\n");
    for (future::KeywordItem* keyword : parser->getKeywords()) {
        printf("%s\n", keyword->getName().c_str());
    }

    printf("\n\n********** All Selectors **********\n");
    for (future::Selector* s : selectors) {
        printf("%s\n", s->description().c_str());
        printf("%s\n\n", s->getRuleData().c_str());
    }
    printf("\n");

    for (future::Selector* s : selectors) {
        // Tranvers DOM tree
        Applicability(gi.get_root_node(), s);
        printf("\n");
    }

    std::list<GumboNode*>allNodes;
    std::list<GumboNode*>& allNodesRef = allNodes;
    future::Selector* s = *selectors.begin();
    AllNodesApplyToSelector(gi.get_root_node(), s, allNodesRef);
    delete[] ptr;
    delete parser;
    return 0;
}

void Applicability(GumboNode* root, future::Selector* selector)
{
    if (root->type == GUMBO_NODE_ELEMENT) {
        GumboVector children = root->v.element.children;
        for (unsigned int i = 0; i < children.length; i++) {
            GumboNode* child = (GumboNode*)(children.data[i]);
            Applicability(child, selector);
        }
        const char* name = HTMLTagNames[root->v.element.tag];
        future::HTMLCSSRefAdaptor::GumboArray nodesArray = &root;
        int* temp = new int(1);
        if (future::HTMLCSSRefAdaptor::nodeAdaptToSelector(&nodesArray, selector, temp)) {
            printf("%s match selector %s line: %d\n",
                name, "YES", root->v.element.start_pos.line);
        }
        delete temp;

    }
    future::HTMLCSSRefAdaptor::CleanResource();
}

void AllNodesApplyToSelector(GumboNode* root, future::Selector* selector, std::list<GumboNode*>& list)
{
    if (root->type == GUMBO_NODE_ELEMENT) {
        GumboVector children = root->v.element.children;
        for (unsigned int i = 0; i < children.length; i++) {
            GumboNode* child = (GumboNode*)(children.data[i]);
            AllNodesApplyToSelector(child, selector, list);
        }
        future::HTMLCSSRefAdaptor::GumboArray nodesArray = &root;
        int* temp = new int(1);
        if (future::HTMLCSSRefAdaptor::nodeAdaptToSelector(&nodesArray, selector, temp)) {
            list.push_back(root);
        }
    }
    future::HTMLCSSRefAdaptor::CleanResource();
}
