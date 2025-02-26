#pragma once

#include <map>

#include <sc-memory/sc_addr.hpp>

namespace formatTranslators
{

class ScAddrComparator;
// this is map that has baseElement as key and another map as value
// value map has <otherElement, connector> pair as key and list of triples as value
// triple <otherElement, connector, isReversed> stores if connector between baseElement and otherElement is reversed
using Triples = ScAddrToValueUnorderedMap<
    std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>>;

class ScAddrComparator
{
public:
  explicit ScAddrComparator(ScAddrToValueUnorderedMap<size_t> & order, Triples & structureTriples)
    : order(order)
    , structureTriples(structureTriples)
  {
  }

  bool operator()(std::pair<ScAddr, ScAddr> const & first, std::pair<ScAddr, ScAddr> const & second) const
  {
    size_t const firstValue = GetValue(first);
    size_t const secondValue = GetValue(second);
    return firstValue < secondValue;
  }

private:
  ScAddrToValueUnorderedMap<size_t> & order;
  Triples & structureTriples;

  size_t GetValue(std::pair<ScAddr, ScAddr> const & key) const
  {
    auto const & directValueIterator = order.find(key.first);
    bool const hasDirectValue = directValueIterator != order.cend();
    auto const & indirectValueIterator = structureTriples.find(key.second);
    bool const hasIndirectValue =
        indirectValueIterator != structureTriples.cend() && !indirectValueIterator->second.empty();
    if (hasIndirectValue)
    {
      ScAddr const & firstOtherElementForConnector = indirectValueIterator->second.begin()->first.first;
      auto const & otherElementOrderIterator = order.find(firstOtherElementForConnector);
      if (otherElementOrderIterator != order.cend())
      {
        if (hasDirectValue)
          return std::min(directValueIterator->second, otherElementOrderIterator->second);
        else
          return otherElementOrderIterator->second;
      }
    }
    if (hasDirectValue)
      return directValueIterator->second;
    else
      return key.first.Hash() + key.second.Hash();
  }
};

}  // namespace formatTranslators
