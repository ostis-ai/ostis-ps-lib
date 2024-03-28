#pragma once

#include "sc-memory/sc_memory.hpp"

namespace nonAtomicActionInterpreterModule
{
class LogicFormulaManager
{
public:
  explicit LogicFormulaManager(ScMemoryContext * context);

  bool checkLogicalFormula(
      ScAddr const & logicFormula,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers);

private:
  ScMemoryContext * context;
};
}  // namespace nonAtomicActionInterpreterModule
