#include "structure_to_triples_parser.hpp"

namespace formatTranslators
{
void StructureToTriplesParser::ParseStructure(
    ScMemoryContext * memoryContext,
    ScAddr const & structureToTranslateAddr,
    ScAddrToValueUnorderedMap<uint32_t> const & keyElementsOrderMap,
    ScAddr const & identifiersLanguageAddr,
    std::shared_ptr<Triples> & structureTriplesMap)
{
  context = memoryContext;
  structureToTranslate = structureToTranslateAddr;
  keyElementsOrder = keyElementsOrderMap;
  identifiersLanguage = identifiersLanguageAddr;
  structureTriples.clear();
  structureTriplesMap->clear();
  ParseStructure();
  structureTriplesMap->insert(structureTriples.begin(), structureTriples.end());
}

void StructureToTriplesParser::ParseStructure()
{
  auto const & structureConnectorsIterator =
      context->CreateIterator3(structureToTranslate, ScType::ConstPermPosArc, ScType::Connector);
  UnorderedTriples unorderedTriples;
  while (structureConnectorsIterator->Next())
  {
    ScAddr const & connector = structureConnectorsIterator->Get(2);
    auto const & [first, second] = context->GetConnectorIncidentElements(connector);
    if (context->GetElementType(first).IsNode())
      unorderedTriples[second].emplace_back(first, connector, true);
    if (context->GetElementType(second).IsNode())
      unorderedTriples[first].emplace_back(second, connector, false);
  }
  OrderTriples(unorderedTriples);
}

void StructureToTriplesParser::OrderTriples(UnorderedTriples const & unorderedTriples)
{
  for (auto const & [baseAddr, triples] : unorderedTriples)
  {
    auto triplesIterator = structureTriples.find(baseAddr);
    if (triplesIterator == structureTriples.cend())
    {
      ScAddrComparator const comparator(keyElementsOrder, unorderedTriples);
      std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>
          pairsWithComparator(comparator);
      triplesIterator = structureTriples.insert({baseAddr, pairsWithComparator}).first;
    }
    for (auto const & [otherElementAddr, connectorAddr, isReversed] : triples)
    {
      if (!keyElementsOrder.count(otherElementAddr))
      {
        // if other element is not key element then it is possible that currently parsed triple is base triple in five
        // elements construction that has key element as fifth element of construction
        auto const & triplesForConnectorIterator = unorderedTriples.find(connectorAddr);
        if (triplesForConnectorIterator == unorderedTriples.cend() || triplesForConnectorIterator->second.empty())
          continue;
        if (std::all_of(
                triplesForConnectorIterator->second.cbegin(),
                triplesForConnectorIterator->second.cend(),
                [this](auto const & otherElementAndConnector)
                {
                  return !keyElementsOrder.count(std::get<0>(otherElementAndConnector));
                }))
          continue;
      }
      triplesIterator->second[{otherElementAddr, connectorAddr}].emplace_back(
          otherElementAddr, connectorAddr, isReversed);
    }
  }
}
}  // namespace formatTranslators
