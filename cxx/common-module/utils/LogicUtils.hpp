#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>

namespace commonModule
{

class LogicUtils
{
public:
  static bool checkLogicalFormula(
      ScMemoryContext * context,
      ScAddr const & logicFormula,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);
};

}  // namespace commonModule
