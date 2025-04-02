This is an agent that translates structure in a format that can be used to visualise structure in SCg.

**Action class:**

`action_visualise_to_scg`

**Parameters:**

1. `structure to translate` - structure that contains graph to translate;
2. `language` - language of identifiers that will be used in translation.

**Comment:**

* If second argument is not passed then `lang_en` is used as default language;
* Structure has to have element with role `rrel_main_key_sc_element` and order of key elements connected to it via relation `nrel_key_elements_order`.

**Workflow:**

* Agent uses order of sc-elements from ordered set connected to structure via relation `nrel_key_elements_order` to create map of type `ScAddrToValueUnorderedMap<uint32_t>` that has key sc-elements as keys and their order as values;
* After that agent iterates through all structure sc-connectors and if source/target sc-element of that triple is node then this triple is added into map `ScAddrToValueUnorderedMap<std::list<std::tuple<ScAddr, ScAddr, bool>>>`. This is map that has `baseElement` as key and list of triples as value. Triple `<otherElement, connector, isReversed>` stores if `connector` between `baseElement` and `otherElement` is reversed.
* After that agent uses triples map and key sc-elements order map to create another map of type `ScAddrToValueUnorderedMap<std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>>`. This is map that has `baseElement` as key and another map as value. Value map has `<otherElement, connector>` pair as key and list of triples as value. Triple `<otherElement, connector, isReversed>` stores if `connector` between `baseElement` and `otherElement` is reversed. `ScAddrComparator` uses key sc-elements order to place key-value pairs into inner map. Order of `otherElement` in that triple and the highest ordered `otherElement` of triples that have `connector` as `baseElement` is used to determine order of inserted key-value pairs.
    * Examples of input structures and maps that will be created after that structure parsing:

| structure                                        | map                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|--------------------------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <img src="../images/structure_to_triples_1.png"> | <code>{class3: {<class2, arc3>: [<class2, arc3, true>], <element1, arc1: [<element1, arc1, false>]>, <element2, arc2: [<element2, arc2, false>]>, <class4, arc7>: [<class4, arc7, false>]}, arc3: {<relation1, arc4>: [<relation1, arc4, true>], <class1, arc5>: [<class1, arc5, false>]>]}, arc5: {<relation2, arc6>: [<relation2, arc6, true>>]}, arc7: {<relation1, arc8>: [<relation1, arc8, true>]}, class2: {<class3, arc3>: [<class3, arc3, false>]}, class4: {<class3, arc7>: [<class3, arc7, true>]}, class1: {<arc5, arc3>: [<arc5, arc3, true>]}}</code> |
| <img src="../images/structure_to_triples_2.png"> | <code>{class3: {<element1, arc1: [<element1, arc1, false>]>, <element2, arc2: [<element2, arc2, false>]>, <class2, arc3>: [<class2, arc3, true>, <class4, arc7, false>]}, arc3: {<relation1, arc4>: [<relation1, arc4, true>], <class1, arc5>: [<class1, arc5, false>]>]}, arc5: {<relation2, arc6>: [<relation2, arc6, true>>]}, arc7: {<relation1, arc8>: [<relation1, arc8, true>]}, class2: {<class3, arc3>: [<class3, arc3, false>]}, class4: {<class3, arc7>: [<class3, arc7, true>]}, class1: {<arc5, arc3>: [<arc5, arc3, true>]}}</code>                   |

{class3: {<class2, arc3>: [<class2, arc3, true>], <element1, arc1: [<element1, arc1, false>]>, <element2, arc2: [<element2, arc2, false>]>, <class4, arc7>: [<class4, arc7, false>]},
arc3: {<relation1, arc4>: [<relation1, arc4, true>], <class1, arc5>: [<class1, arc5, false>]>]},
arc5: {<relation2, arc6>: [<relation2, arc6, true>>]},
arc7: {<relation1, arc8>: [<relation1, arc8, true>]},
class2: {<class3, arc3>: [<class3, arc3, false>]},
class4: {<class3, arc7>: [<class3, arc7, true>]},
class1: {<arc5, arc3>: [<arc5, arc3, true>]}}


{class3: {<element1, arc1: [<element1, arc1, false>]>, <element2, arc2: [<element2, arc2, false>]>, <class2, arc3>: [<class2, arc3, true>, <class4, arc7, false>]},
arc3: {<relation1, arc4>: [<relation1, arc4, true>], <class1, arc5>: [<class1, arc5, false>]>]},
arc5: {<relation2, arc6>: [<relation2, arc6, true>>]},
arc7: {<relation1, arc8>: [<relation1, arc8, true>]},
class2: {<class3, arc3>: [<class3, arc3, false>]},
class4: {<class3, arc7>: [<class3, arc7, true>]},
class1: {<arc5, arc3>: [<arc5, arc3, true>]}}

* After that agent translates triples to model objects in order to allow further coordinates assigning. That model tree at this step contains sc-types, sc-addresses, identifiers and incidence of sc-elements Classes that are used in the hierarchy:

<img src="../images/model_objects_hierarchy.png"></img>

* After that agent traverses model tree to assign X coordinates to its nodes and split tree into multiple trees if X coordinate becomes too large(max value is configured via `formatTranslators::FormatTranslatorsConstants::MAX_X`);
    * X coordinate is calculated based on current level identifier length and content size if it is an sc-link, identifier lengths of connectors(connectors1) to next level elements, identifier lengths of connectors(connectors2) to connectors1, identifier lengths of other elements of connectors2 and content sizes of next level elements if they are sc-links;
    * If X coordinate becomes too large and next level elements have connectors to other elements then copy of next level element that have connectors is substituted instead of next level element (all fields but connectors are copied) and original next level element is added to collection of nodes that will start X coordinates assigning from minimal value which is configured via `formatTranslators::FormatTranslatorsConstants::MIN_X`;
    * Constant `formatTranslators::FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT` is used to calculate incidence point between horizontal and vertical connectors;
* After that agent assigns Y coordinates to all subtrees and uses visitor to visit and translate them to gwf format;
    * One variable is reused for all subtrees to place them one under the other, which is incremented after level element without connectors is processed to create gap between horizontal connectors and before level element with bus is processed (level element has bus if it has more than one connector) to create gap between level element and the first connector;
    * Constant `formatTranslators::FormatTranslatorsConstants::Y_INCREMENT` is used to set vertical gap between connectors;
    * Constant `formatTranslators::FormatTranslatorsConstants::HALF_Y_INCREMENT` is used to calculate vertical gap between horizontal connectors and relations;
    * Each translation assigns ids to elements starting with id = 0;
    * Buses have suffix "_bus" in their ids to create distinct ids for nodes and their corresponding buses;
    * Constant `formatTranslators::GWFTranslatorVisitor::NODE_TYPES` contains all supported node types;
    * Constant `formatTranslators::GWFTranslatorVisitor::CONNECTOR_TYPES` contains all supported connector types;
* Constructed gwf file is set as content of generated sc-link and that sc-link is added to action result structure.

### Example

Example of an input structure:

<img src="../images/scg_visualisation_translator_agent_input.png"></img>

Example of an output structure:

<img src="../images/scg_visualisation_translator_agent_output.png"></img>

### Agent implementation language
C++

### Result

Possible result codes:

* `sc_result_ok` - structure is translated successfully;
* `sc_result_error` - internal error.
