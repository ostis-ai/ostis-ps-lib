#pragma once

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_module.hpp"

#include "keynodes/Keynodes.hpp"
#include "agent/NonAtomicActionInterpreterAgent.hpp"

#include "NonAtomicActionInterpreterModule.generated.hpp"
#include "utils/ActionUtils.hpp"

namespace nonAtomicActionInterpreterModule
{
class NonAtomicActionInterpreterModule : public ScModule
{
  SC_CLASS(LoadOrder(100))
  SC_GENERATED_BODY()

  virtual sc_result

  InitializeImpl() override;

  virtual sc_result ShutdownImpl() override;
};

}  // namespace nonAtomicActionInterpreterModule
