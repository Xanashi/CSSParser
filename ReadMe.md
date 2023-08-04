# CSSParser++

A C++ CSS Parser

[![LISCENSE](https://img.shields.io/packagist/l/doctrine/orm.svg)](./LICENSE.md)

### This is a fork of luojilab´s [DDCSSParser](https://github.com/luojilab/CSSParser "DDCSSParser").<br>Full credits goes to [luojilab](https://github.com/luojilab "luojilab") for writing the CSS Parser and Gumbo code.
------

CSSParser++ is a C++ CSS parser and selector, that can read full CSS stylesheets or standalone CSS strings and match the rules with a custom node tree provided by the 
implementing library/application.

Please check the CustomNodeExample project for a full example.

## Changes from DDCSSParser

- Project has been converted to a Visual Studio solution
- Code has been split into 3 Visual Studio projects 
  - A CSSParser library - this outputs a lib file that can be used by implementing libraries/applications
  - A CustomNodeExample console application - Example that showcases how to provide a custom node class to the CSSParser and retrieve the matching style rules for each node
  - A GumboExample console application - the original example from DDCSSParser
- The CSSParser library is no longer dependent on Gumbo
- A new API based on the INodeSelector interface has been added, allowing implementing libraries/applications to provide their own custom node tree to the node-selector resolvers.
- Various minor changes and fixes


## Installation

##### Using the Code

* Clone the source code into a working directory `git clone https://github.com/Xanashi/CSSParser.git`
* Open the Visual Studio solution
* Build the CSSParser library (in Release mode)
* Link the compiled cssparser.lib in your project, and add the "CssParser\include" directory to your include path. 

##### Running the example Apps

* Open the Visual Studio solution
* Build All
* Run either the CustomNodeExample or the GumboExample

## Features

* Parse CSS files or strings
* Easily iterate through the CSS selectors and rules
* CSS rules is provided as raw string or std::map key/value pairs
* Use INodeSelector interface to match a custom node tree automatically
with the CSS selectors. 

#### Supported CSS Selectors:

- Simple Selector:
  - Type Selector
  - Universal Selector
  - Attribute Selector
  - Class Selector
  - ID Selector
  - Pseudo Class
- Selector Sequence
- Selector Group
- Combinators:
  - Descendant combinator
  - Child combinators
  - Sibling combinators


## Basic Usage

* Parse CSS files or strings

  ```c++
  // Parse CSS file
  future::CSSParser* parser = new future::CSSParser();
  parser->parseByFile("/Path/to/CssFile");
  std::vector<future::Selector *>selectors = parser>getSelectors();
  
  // Parse CSS string
  parser->parseByString("body #id { color: black; }");
  std::vector<future::Selector *>selectors = parser>getSelectors();
  ```

* Print all selector descriptions and rules

  ```c++
  int i = 1;
  printf("\n\n********** All Selectors **********\n\n");
  for (future::Selector* s : selectors) {
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
  ```

* Match custom node with selector and handle selector rules and specificity if the node macthes a selector (See the CustomNodeExample for the full code)

  ```c++
  int selIdx = 1;
  CustomNode* node = new CustomNode(); // <-- Your custom node 
  for (future::Selector* s : selectors) {
      if (future::NodeSelectorResolver::DoesNodeMatchSelector(node, s)) {
          auto spec = s->weight();
          auto ruleMap = s->getRuleDataMap();

          ApplyStyleRules(node, ruleMap, spec);

          printf("node %s%s %s matched with selector #%i\n", 
          node->tag.c_str(), node->GetIdDesc().c_str(), node->GetClassesDesc().c_str(), selIdx);
      }
      selIdx++;
  }
  ```

## Applicability

The code has been tested on Windows
