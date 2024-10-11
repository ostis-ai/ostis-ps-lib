#pragma once

#include <sc-memory/sc_agent.hpp>
#include "sc-agents-common/utils/IteratorUtils.hpp"

namespace nonAtomicActionInterpreterModuleTest
{
class AssignDynamicArgumentTestAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;
};

}  // namespace nonAtomicActionInterpreterModuleTest
