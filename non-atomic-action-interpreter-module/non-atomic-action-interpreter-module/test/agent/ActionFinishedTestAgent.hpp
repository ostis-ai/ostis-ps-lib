#pragma once

#include <sc-memory/sc_agent.hpp>

namespace nonAtomicActionInterpreterModuleTest
{
class ActionFinishedTestAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;

  ScResult DoProgram(ScAction & action) override;
};

}  // namespace nonAtomicActionInterpreterModuleTest
