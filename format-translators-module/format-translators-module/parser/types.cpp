#include "types.hpp"

namespace formatTranslators
{

ScAddrComparator::ScAddrComparator(
    ScAddrToValueUnorderedMap<uint32_t> const & order,
    UnorderedTriples const & structureTriples)
  : order(order)
  , structureTriples(structureTriples)
{
}

bool ScAddrComparator::operator()(std::pair<ScAddr, ScAddr> const & first, std::pair<ScAddr, ScAddr> const & second)
    const
{
  uint32_t const firstValue = GetValue(first);
  uint32_t const secondValue = GetValue(second);
  return firstValue < secondValue;
}

uint32_t ScAddrComparator::GetValue(std::pair<ScAddr, ScAddr> const & key) const
{
  auto const & directValueIterator = order.find(key.first);
  bool const hasDirectValue = directValueIterator != order.cend();
  auto const & indirectValueIterator = structureTriples.find(key.second);
  bool const hasIndirectValue =
      indirectValueIterator != structureTriples.cend() && !indirectValueIterator->second.empty();
  if (hasIndirectValue)
  {
    bool foundIndirectValueInOrder = false;
    uint32_t minimalValueInOrder = order.size();
    for (auto const & otherElementAndConnector : indirectValueIterator->second)
    {
      auto const & otherElementOrderIterator = order.find(std::get<0>(otherElementAndConnector));
      if (otherElementOrderIterator != order.cend())
      {
        foundIndirectValueInOrder = true;
        if (otherElementOrderIterator->second < minimalValueInOrder)
          minimalValueInOrder = otherElementOrderIterator->second;
      }
    }
    if (foundIndirectValueInOrder)
    {
      if (hasDirectValue)
        return std::min(directValueIterator->second, minimalValueInOrder);
      else
        return minimalValueInOrder;
    }
  }
  if (hasDirectValue)
    return directValueIterator->second;
  else
    return key.first.Hash() + key.second.Hash();
}

}  // namespace formatTranslators
