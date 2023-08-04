
#include "CustomNode.h"
#include <resolvers/NodeSelectorResolver.h>
#include <cssparser/CSSParser.h>
#include <utils/CSSUtil.h>

#include <iostream>

void GetNodeStyles(CustomNode* node, std::vector<future::Selector*> selectors);
void ApplyStyleRules(CustomNode* node, std::map<std::string, std::string>& ruleMap, int specificity);
CustomNode* GenerateNodeTree();
std::string GetCssString();
void PrintSelectors(future::CSSParser* parser);

int main()
{
    auto root = GenerateNodeTree();
    auto css  = GetCssString();

    future::CSSParser* parser = new future::CSSParser;
    parser->parseByString(css);

    PrintSelectors(parser);

    GetNodeStyles(root, parser->getSelectors());

    delete parser;
    delete root;
}

void GetNodeStyles(CustomNode* node, std::vector<future::Selector*> selectors)
{
    for (const auto& child : node->children) {
        GetNodeStyles(child, selectors);
    }

    // inline style
    if (!node->inlineStyle.empty()) {
        auto inlineStyleMap = future::CSSUtil::GetRuleMapFromString(node->inlineStyle);
        ApplyStyleRules(node, inlineStyleMap, 1000);
    }

    int selIdx = 1;
    for (future::Selector* s : selectors) {
        if (future::NodeSelectorResolver::DoesNodeMatchSelector(node, s)) {
             auto spec    = s->weight();
             auto ruleMap = s->getRuleDataMap();

             ApplyStyleRules(node, ruleMap, spec);

             printf("node %s%s %s matched with selector #%i\n", 
                 node->tag.c_str(), node->GetIdDesc().c_str(), node->GetClassesDesc().c_str(), selIdx);
        }
        selIdx++;
    }

    printf("\n");
}

void ApplyStyleRules(CustomNode* node, std::map<std::string, std::string>& ruleMap, int specificity)
{
    auto& styles = node->styles;

    for (const auto& rule : ruleMap) {
        auto& key = rule.first;
        if (!styles.Contains(key) || specificity > styles.GetSpecificity(key))
            styles.AddOrOverwrite(key, rule.second, specificity);
    }
}

CustomNode* GenerateNodeTree()
{
    return 
    new CustomNode("html", "root", "container firstnode", {}, "", 
    {
        new CustomNode("body", "main", "container main-box", {}, "",
        {
            new CustomNode("div", "s1", "section one", { {"data-feel", "serene"} }, "",
            {
                new CustomNode("h1", "", "title", {}, "",
                {
                }),
                new CustomNode("h2", "", "subtitle", { {"target", ""} }, "",
                {
                }),
                new CustomNode("div", "", "text-body", {}, "",
                {
                    new CustomNode("p", "", "description", {}, "",
                    {
                    })
                })
            }),
            new CustomNode("div", "s2", "section two", { {"data-feel", "content"} }, "",
            {
                new CustomNode("h3", "", "", {}, "",
                {
                    new CustomNode("p", "", "inner-paragraph", {}, "",
                    {
                    })
                }),
                new CustomNode("p", "", "paragraph", {}, "",
                {
                }),
                new CustomNode("p", "", "paragraph", {}, "font-family: Test; font-size: 10px;",
                {
                }),
                new CustomNode("p", "", "paragraph", {}, "",
                {
                })
            }),
            new CustomNode("div", "s3", "section three", { {"data-feel", "love"} }, "",
            {
                new CustomNode("ul", "", "list", {}, "",
                {
                    new CustomNode("li", "", "listitem", {}, "",
                    {
                    }),
                    new CustomNode("li", "", "listitem", {}, "background: whitesmoke;",
                    {
                    }),
                    new CustomNode("li", "", "listitem", {}, "",
                    {
                    })
                })
            })
        })
    });
}

std::string GetCssString()
{
    std::string s(
        "@font-face { font-family: Test; font-style: normal; font-weight: 400; font-stretch: normal; font-variant: normal; font-feature-settings: normal;"
        "src: url('data:font/ttf;base64,AAEAAAASAQAABAAgR0RFRpBWgnUAAZQAAAABpkdQT1PvVcM1AAGVqAAAL7BHU1VCq/Re7wABxVgAAAzKT1MvMqpWvf8AAVyIA...'); }");

    return s + R"(
        body { 
          font-family: 'Helvetica';
          text-align: center; 
        }

        .main-box{ 
          display: flex;
          flex-wrap: wrap;
        }

        p {
          color: black;
        }   

        .one, .two, .three { 
          flex-basis: 40%;
        }

        div { 
          border: 1px lightgrey dotted;
          margin: 30px;
        }

        h1, h2 { 
          color: purple;
        }

        li + li { 
          color: pink;
        }

        h3 ~ p:nth-child(3n+1) { 
          font-size: 40px;
        }

        .section div { 
          color: blue;
        }

        .section h3 { 
          color: darkgreen;
        }

        .section > h3 { 
          color: darkblue;
        }

        .section > h3 > p { 
          color: darkred;
        }

        .main-box .one {
          flex-basis: 30%;
        }

        div.two {
          color: blue;
        }

        .three[data-feel='love'] {
          transform: rotate(45deg);
        }
    )";
}

void PrintSelectors(future::CSSParser* parser)
{
    int i = 1;
    printf("\n\n********** All Selectors **********\n\n");
    for (future::Selector* s : parser->getSelectors()) {
        printf("#%i\n\n", i++);
        printf("%s\n\n", s->description().c_str());
        printf("specificity: %i\n\n\n", s->weight());

        auto ruleMap = s->getRuleDataMap();
        for (const auto& rule : ruleMap) {
            printf("%s: %s\n", rule.first.c_str(), rule.second.c_str());
        }

        printf("\n----------------------\n\n");
    }
    printf("\n");
}