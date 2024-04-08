#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_memory.hpp>

namespace commonModule
{

class ActionUtils
{
public:
  static bool initAgentAndWaitForActionSuccessfulFinish(
      ScMemoryContext * context,
      ScAddr const & actionAddr,
      int const & waitTime);

  static bool isActionDeactivated(ScMemoryContext * context, ScAddr const & actionAddr);
};

}  // namespace commonModule
