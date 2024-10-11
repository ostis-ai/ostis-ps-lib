#include <algorithm>

#include "RelationUtils.hpp"

using namespace commonModule;

void RelationUtils::eraseAllEdges(
    ScMemoryContext * context,
    ScAddr const & source,
    ScAddr const & target,
    ScType const & edgeType)
{
  ScIterator3Ptr iterator3 = context->CreateIterator3(source, edgeType, target);
  while (iterator3->Next())
  {
    context->EraseElement(iterator3->Get(1));
  }
}

bool RelationUtils::checkRelationBetween(
    ScMemoryContext * context,
    ScAddr const & begin,
    ScAddr const & end,
    ScAddr const & relation)
{
  ScIterator5Ptr iterator5 = context->CreateIterator5(begin, ScType::Unknown, end, ScType::Unknown, relation);
  return iterator5->Next();
}

bool RelationUtils::checkAllInEdges(
    ScMemoryContext * context,
    ScAddr const & node,
    ScType const & edgeType,
    ScAddrVector const & addrVector)
{
  return std::all_of(addrVector.cbegin(), addrVector.cend(), [context, &node, &edgeType](auto const & addr) {
    return context->CheckConnector(addr, node, edgeType);
  });
}

bool RelationUtils::checkAllOutEdges(
    ScMemoryContext * context,
    ScAddr const & node,
    ScType const & edgeType,
    ScAddrVector const & addrVector)
{
  return std::all_of(addrVector.cbegin(), addrVector.cend(), [context, &node, &edgeType](auto const & addr) {
    return context->CheckConnector(node, addr, edgeType);
  });
}
