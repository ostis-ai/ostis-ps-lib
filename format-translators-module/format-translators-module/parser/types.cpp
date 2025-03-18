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
    ScAddr const & firstOtherElementForConnector = std::get<0>(*indirectValueIterator->second.begin());
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

}  // namespace formatTranslators
