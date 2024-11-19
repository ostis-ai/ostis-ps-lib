#pragma once

#include <sc-memory/sc_keynodes.hpp>

#include "sc-memory/sc_addr.hpp"
#include "sc-memory/sc_object.hpp"

namespace nonAtomicActionInterpreterModule
{
class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_interpret_non_atomic_action{"action_interpret_non_atomic_action"};

  static inline ScKeynode const nrel_decomposition_of_action{"nrel_decomposition_of_action"};

  static inline ScKeynode const nrel_then{"nrel_then"};

  static inline ScKeynode const nrel_else{"nrel_else"};

  static inline ScKeynode const nrel_goto{"nrel_goto"};

  static inline ScKeynode const nrel_condition{"nrel_condition"};

  static inline ScKeynode const nrel_priority_path{"nrel_priority_path"};

  static inline ScKeynode const nrel_basic_sequence{"nrel_basic_sequence"};

  static inline ScKeynode const action_cancelled{"action_cancelled"};

  static inline ScKeynode const nrel_subaction{"nrel_subaction"};
};

}  // namespace nonAtomicActionInterpreterModule
