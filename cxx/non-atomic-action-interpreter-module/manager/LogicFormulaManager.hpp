#pragma once

#include "sc-memory/sc_memory.hpp"

namespace nonAtomicActionInterpreterModule
{
class LogicFormulaManager
{
public:
  bool checkLogicalFormula(
      ScMemoryContext * context,
      ScAddr const & logicFormula,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);
};
}  // namespace nonAtomicActionInterpreterModule
