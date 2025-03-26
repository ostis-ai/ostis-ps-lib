#pragma once

#include <sc-memory/sc_memory.hpp>

namespace common
{

class RelationUtils
{
public:
  static void EraseAllConnectors(
      ScMemoryContext * context,
      ScAddr const & source,
      ScAddr const & target,
      ScType const & connectorType);

  static bool CheckRelationBetween(
      ScMemoryContext * context,
      ScAddr const & source,
      ScAddr const & target,
      ScAddr const & relation);

  static bool CheckAllIncomingConnectors(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScType const & connectorType,
      ScAddrVector const & addrVector);

  static bool CheckAllOutgoingConnectors(
      ScMemoryContext * ms_context,
      ScAddr const & node,
      ScType const & connectorType,
      ScAddrVector const & addrVector);
};

}  // namespace common
