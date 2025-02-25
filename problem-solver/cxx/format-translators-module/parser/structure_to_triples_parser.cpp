#include "structure_to_triples_parser.hpp"

namespace formatTranslators
{
void StructureToTriplesParser::ParseStructure(
    ScMemoryContext * memoryContext,
    ScAddr const & structureToTranslateAddr,
    ScAddrToValueUnorderedMap<size_t> const & keyElementsOrderMap,
    ScAddr const & identifiersLanguageAddr,
    Triples & structureTriplesMap)
{
  context = memoryContext;
  structureToTranslate = structureToTranslateAddr;
  keyElementsOrder = keyElementsOrderMap;
  identifiersLanguage = identifiersLanguageAddr;
  structureTriples.clear();
  structureTriplesMap.clear();
  ParseStructure();
  structureTriplesMap.insert(structureTriples.begin(), structureTriples.end());
}

void StructureToTriplesParser::ParseStructure()
{
  auto const & structureConnectorsIterator =
      context->CreateIterator3(structureToTranslate, ScType::ConstPermPosArc, ScType::Connector);
  std::list<std::tuple<ScAddr, ScAddr, ScAddr>> triplesWithConnectors;
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> triplesWithConnectorNotInParsedStructure;
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> triplesWithConnectorInParsedStructure;
  while (structureConnectorsIterator->Next())
  {
    ScAddr const & connector = structureConnectorsIterator->Get(2);
    auto const & [first, second] = context->GetConnectorIncidentElements(connector);
    ParseTriple(
        first,
        connector,
        second,
        false,
        triplesWithConnectorNotInParsedStructure,
        triplesWithConnectorInParsedStructure);
    ParseTriple(
        second,
        connector,
        first,
        true,
        triplesWithConnectorNotInParsedStructure,
        triplesWithConnectorInParsedStructure);
  }
  for (auto const & [connector, firstAndSecond] : triplesWithConnectorInParsedStructure)
  {
    if (structureTriples.count(connector))
    {
      auto const & [first, second] = firstAndSecond;
      if (context->GetElementType(second).IsNode())
        AddTripleToParsedTriplesIfOtherIsNode(first, connector, second, false);
      if (context->GetElementType(first).IsNode())
        AddTripleToParsedTriplesIfOtherIsNode(second, connector, first, true);
    }
  }
}

void StructureToTriplesParser::ParseTriple(
    ScAddr const & baseAddr,
    ScAddr const & connectorAddr,
    ScAddr const & otherElementAddr,
    bool const isReversed,
    ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & triplesWithConnectorNotInParsedStructure,
    ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> & triplesWithConnectorInParsedStructure)
{
  auto const sourceTargetPair =
      isReversed ? std::make_pair(otherElementAddr, baseAddr) : std::make_pair(baseAddr, otherElementAddr);
  if (context->GetElementType(otherElementAddr).IsNode())
  {
    if (keyElementsOrder.count(otherElementAddr))
    {
      AddTripleToParsedTriplesIfOtherIsNode(baseAddr, connectorAddr, otherElementAddr, isReversed);
      auto const & notInParsedIterator = triplesWithConnectorNotInParsedStructure.find(connectorAddr);
      if (notInParsedIterator != triplesWithConnectorNotInParsedStructure.cend())
      {
        triplesWithConnectorInParsedStructure.insert({connectorAddr, sourceTargetPair});
        triplesWithConnectorNotInParsedStructure.erase(connectorAddr);
      }
    }
    else
    {
      // if other element is not key element then it is possible that currently parsed triple is base triple in five
      // elements construction that has key element as fifth element of construction
      if (structureTriples.count(connectorAddr))
        triplesWithConnectorInParsedStructure.insert({connectorAddr, sourceTargetPair});
      else
        triplesWithConnectorNotInParsedStructure.insert({connectorAddr, sourceTargetPair});
    }
  }
}

void StructureToTriplesParser::AddTripleToParsedTriplesIfOtherIsNode(
    ScAddr const & baseAddr,
    ScAddr const & connectorAddr,
    ScAddr const & otherElementAddr,
    bool const isReversed)
{
  if (context->GetElementType(otherElementAddr).IsNode())
  {
    if (!structureTriples.count(baseAddr))
    {
      ScAddrComparator const comparator(keyElementsOrder, structureTriples);
      std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>
          pairsWithComparator(comparator);
      structureTriples.insert({baseAddr, pairsWithComparator});
    }
    structureTriples.at(baseAddr)[{otherElementAddr, connectorAddr}].emplace_back(
        otherElementAddr, connectorAddr, isReversed);
  }
}
}  // namespace formatTranslators
