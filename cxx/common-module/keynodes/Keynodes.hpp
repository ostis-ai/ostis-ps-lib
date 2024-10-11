#pragma once

#include <sc-memory/sc_keynodes.hpp>

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

namespace commonModule
{
class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_deactivated{"action_deactivated"};

  static inline ScKeynode const atomic_logical_formula{"atomic_logical_formula"};
};

}  // namespace commonModule
