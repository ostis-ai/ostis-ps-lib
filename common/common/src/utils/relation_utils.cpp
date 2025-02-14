#include <algorithm>

#include "common/utils/relation_utils.hpp"

using namespace common;

void RelationUtils::EraseAllConnectors(
    ScMemoryContext * context,
    ScAddr const & source,
    ScAddr const & target,
    ScType const & connectorType)
{
  ScIterator3Ptr iterator3 = context->CreateIterator3(source, connectorType, target);
  while (iterator3->Next())
  {
    context->EraseElement(iterator3->Get(1));
  }
}

bool RelationUtils::CheckRelationBetween(
    ScMemoryContext * context,
    ScAddr const & source,
    ScAddr const & target,
    ScAddr const & relation)
{
  ScIterator5Ptr iterator5 = context->CreateIterator5(source, ScType::Unknown, target, ScType::Unknown, relation);
  return iterator5->Next();
}

bool RelationUtils::CheckAllIncomingConnectors(
    ScMemoryContext * context,
    ScAddr const & node,
    ScType const & connectorType,
    ScAddrVector const & addrVector)
{
  return std::all_of(
      addrVector.cbegin(),
      addrVector.cend(),
      [context, &node, &connectorType](auto const & addr)
      {
        return context->CheckConnector(addr, node, connectorType);
      });
}

bool RelationUtils::CheckAllOutgoingConnectors(
    ScMemoryContext * context,
    ScAddr const & node,
    ScType const & connectorType,
    ScAddrVector const & addrVector)
{
  return std::all_of(
      addrVector.cbegin(),
      addrVector.cend(),
      [context, &node, &connectorType](auto const & addr)
      {
        return context->CheckConnector(node, addr, connectorType);
      });
}
