#pragma once

#include <sc-memory/sc_memory.hpp>

namespace common
{

class LogicUtils
{
public:
  static bool CheckLogicalFormula(
      ScMemoryContext * context,
      ScAddr const & logicFormula,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);
};

}  // namespace common
