#pragma once

#include <map>

#include <sc-memory/sc_addr.hpp>

namespace formatTranslators
{

using UnorderedTriples = ScAddrToValueUnorderedMap<std::list<std::tuple<ScAddr, ScAddr, bool>>>;

class ScAddrComparator;
// this is map that has baseElement as key and another map as value
// value map has <otherElement, connector> pair as key and list of triples as value
// triple <otherElement, connector, isReversed> stores if connector between baseElement and otherElement is reversed
using Triples = ScAddrToValueUnorderedMap<
    std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>>;

class ScAddrComparator
{
public:
  explicit ScAddrComparator(
      ScAddrToValueUnorderedMap<uint32_t> const & order,
      UnorderedTriples const & structureTriples);

  bool operator()(std::pair<ScAddr, ScAddr> const & first, std::pair<ScAddr, ScAddr> const & second) const;

private:
  ScAddrToValueUnorderedMap<uint32_t> const & order;
  UnorderedTriples const & structureTriples;

  uint32_t GetValue(std::pair<ScAddr, ScAddr> const & key) const;
};

}  // namespace formatTranslators
