#pragma once

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

#include "Keynodes.generated.hpp"

namespace nonAtomicActionInterpreterModule
{
class Keynodes : public ScObject
{
  SC_CLASS()
  SC_GENERATED_BODY()

public:
  SC_PROPERTY(Keynode("action_deactivated"), ForceCreate)
  static ScAddr action_deactivated;

  SC_PROPERTY(Keynode("action_interpret_non_atomic_action"), ForceCreate)
  static ScAddr action_interpret_non_atomic_action;

  SC_PROPERTY(Keynode("nrel_decomposition_of_action"), ForceCreate)
  static ScAddr nrel_decomposition_of_action;

  SC_PROPERTY(Keynode("nrel_then"), ForceCreate)
  static ScAddr nrel_then;

  SC_PROPERTY(Keynode("nrel_else"), ForceCreate)
  static ScAddr nrel_else;

  SC_PROPERTY(Keynode("nrel_goto"), ForceCreate)
  static ScAddr nrel_goto;

  SC_PROPERTY(Keynode("nrel_condition"), ForceCreate)
  static ScAddr nrel_condition;

  SC_PROPERTY(Keynode("nrel_priority_path"), ForceCreate)
  static ScAddr nrel_priority_path;

  SC_PROPERTY(Keynode("nrel_basic_sequence"), ForceCreate)
  static ScAddr nrel_basic_sequence;

  SC_PROPERTY(Keynode("atomic_logical_formula"), ForceCreate)
  static ScAddr atomic_logical_formula;

  SC_PROPERTY(Keynode("action_cancelled"), ForceCreate)
  static ScAddr action_cancelled;

  SC_PROPERTY(Keynode("nrel_subaction"), ForceCreate)
  static ScAddr nrel_subaction;
};

}  // namespace nonAtomicActionInterpreterModule
