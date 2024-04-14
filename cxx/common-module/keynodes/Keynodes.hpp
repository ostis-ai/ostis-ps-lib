#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "Keynodes.generated.hpp"

namespace commonModule
{
class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_deactivated"), ForceCreate)
  static ScAddr action_deactivated;

  SC_PROPERTY(Keynode("atomic_logical_formula"), ForceCreate)
  static ScAddr atomic_logical_formula;
};

}  // namespace commonModule
