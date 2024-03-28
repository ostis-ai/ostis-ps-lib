#include <algorithm>

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "RelationUtils.hpp"

ScAddr RelationUtils::getIndexRelation(ScMemoryContext * context, int const & index)
{
  if (index <= 0)
  {
    throw std::runtime_error("Invalid index.");
  }
  if (index == 1)
  {
    return scAgentsCommon::CoreKeynodes::rrel_1;
  }
  else if (index == 2)
  {
    return scAgentsCommon::CoreKeynodes::rrel_2;
  }
  else
  {
    return context->HelperFindBySystemIdtf("rrel_" + std::to_string(index));
  }
}

void RelationUtils::eraseAllEdges(
    ScMemoryContext * context,
    const ScAddr & source,
    const ScAddr & target,
    const ScType & edgeType)
{
  ScIterator3Ptr iterator3 = context->Iterator3(source, edgeType, target);
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
  ScIterator5Ptr iterator5 = context->Iterator5(begin, ScType::Unknown, end, ScType::Unknown, relation);
  return iterator5->Next();
}

bool RelationUtils::checkAllInEdges(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScType const & edgeType,
    ScAddrVector const & addrVector)
{
  return std::all_of(addrVector.cbegin(), addrVector.cend(), [ms_context, &node, &edgeType](auto const & addr) {
    return ms_context->HelperCheckEdge(addr, node, edgeType);
  });
}

bool RelationUtils::checkAllOutEdges(
    ScMemoryContext * ms_context,
    ScAddr const & node,
    ScType const & edgeType,
    ScAddrVector const & addrVector)
{
  return std::all_of(addrVector.cbegin(), addrVector.cend(), [ms_context, &node, &edgeType](auto const & addr) {
    return ms_context->HelperCheckEdge(node, addr, edgeType);
  });
}

ScAddr RelationUtils::setOutValue(
    ScMemoryContext * context,
    ScAddr const & begin,
    ScType const & edgeType,
    ScAddr const & end,
    ScAddr const & relation)
{
  context->ForEachIter5(
      begin,
      edgeType,
      ScType::Unknown,
      ScType::Unknown,
      relation,
      [context](
          ScAddr const & src, ScAddr const & edge, ScAddr const & trg, ScAddr const & edgeAttr, ScAddr const & attr) {
        context->EraseElement(edge);
      });
  ScAddr const & edge = context->CreateEdge(edgeType, begin, end);
  context->CreateEdge(ScType::EdgeAccessConstPosPerm, relation, edge);
  return edge;
}

ScAddr RelationUtils::setInValue(
    ScMemoryContext * context,
    ScAddr const & begin,
    ScType const & edgeType,
    ScAddr const & end,
    ScAddr const & relation)
{
  context->ForEachIter5(
      ScType::Unknown,
      edgeType,
      end,
      ScType::Unknown,
      relation,
      [context](
          ScAddr const & src, ScAddr const & edge, ScAddr const & trg, ScAddr const & edgeAttr, ScAddr const & attr) {
        context->EraseElement(edge);
      });
  ScAddr const & edge = context->CreateEdge(edgeType, begin, end);
  context->CreateEdge(ScType::EdgeAccessConstPosPerm, relation, edge);
  return edge;
}
