#pragma once

#include <map>

#include <sc-memory/sc_addr.hpp>

namespace formatTranslators
{

class ScAddrComparator;
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
    if (order.count(key.first))
      return order.at(key.first);
    ScAddr const & firstOtherElementForConnector = structureTriples.at(key.second).begin()->first.first;
    if (order.count(firstOtherElementForConnector))
      return order.at(firstOtherElementForConnector);
    return key.first.Hash() + key.second.Hash();
  }
};

}  // namespace formatTranslators
