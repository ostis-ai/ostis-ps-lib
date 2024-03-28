#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_addr.hpp"

class RelationUtils
{
public:
  static ScAddr getIndexRelation(ScMemoryContext * context, int const & index);

  static void eraseAllEdges(
      ScMemoryContext * context,
      ScAddr const & source,
      ScAddr const & target,
      ScType const & edgeType);

  static bool checkRelationBetween(
      ScMemoryContext * context,
      ScAddr const & begin,
      ScAddr const & end,
      ScAddr const & relation);

  static bool checkAllInEdges(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScType const & edgeType,
      ScAddrVector const & addrVector);

  static bool checkAllOutEdges(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScType const & edgeType,
      ScAddrVector const & addrVector);

  static ScAddr setOutValue(
      ScMemoryContext * context,
      ScAddr const & begin,
      ScType const & edgeType,
      ScAddr const & end,
      ScAddr const & relation);

  static ScAddr setInValue(
      ScMemoryContext * context,
      ScAddr const & begin,
      ScType const & edgeType,
      ScAddr const & end,
      ScAddr const & relation);
};
