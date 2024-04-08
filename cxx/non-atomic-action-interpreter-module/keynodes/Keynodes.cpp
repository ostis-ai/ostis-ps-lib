#include "Keynodes.hpp"

#include "sc-memory/sc_memory.hpp"

using namespace nonAtomicActionInterpreterModule;

ScAddr Keynodes::action_interpret_non_atomic_action;
ScAddr Keynodes::nrel_decomposition_of_action;
ScAddr Keynodes::nrel_then;
ScAddr Keynodes::nrel_else;
ScAddr Keynodes::nrel_goto;
ScAddr Keynodes::nrel_condition;
ScAddr Keynodes::nrel_priority_path;
ScAddr Keynodes::nrel_basic_sequence;
ScAddr Keynodes::atomic_logical_formula;
ScAddr Keynodes::action_cancelled;
ScAddr Keynodes::nrel_subaction;
