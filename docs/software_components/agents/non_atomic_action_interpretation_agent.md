Is an agent that generates an action description in the knowlendge base based on the template (program) received. If there are sets of arguments then it performs a role mapping before generation.

**Action class:**

`action_interpret_non_atomic_action`

**Parameters:**

1. `program` - a template for creating a process (non-atomic action) description.
2. **(optional)** `set of arguments`. If there is no need to pass arguments to a non-atomic action, then passing an empty set or the absence of this argument is acceptable.
3. `general action` - action instance relates to interpretation non-atomic action instance that initiate it by relation `nrel_subaction`. 

### Workflow

* The first argument of the non-atomic action is replaced by the node that belongs to the set of arguments as the first, the second argument of the non-atomic action is replaced by the node belongs to the set as the second. Note that The order of the arguments is given by the relations `rrel_1`, `rrel_2`, and so on, not by the sequence relation;
* In the course of an agent's work, the `template` program generates a description of a non-atomic action. The atomic actions composing it are also added to the class of performed (successfully or unsuccessfully) by the agents that performed them.

### Comment
1. Before initiating each atomic action, there is a check to see if the `general action` is interrupted.
   If the `general action` has been interrupted, then the interpretation of non-atomic action stops.

### Example

Example of an input structure:

<img src="../images/non_atomic_action_interpretation_agent_input.png"></img>

Example of an output structure:

<img src="../images/non_atomic_action_interpretation_agent_output.png"></img>

Example of an program:

<img src="../images/non_atomic_action_interpretation_agent_example.png"></img>

### Transitions

#### Transition by action result

The transition to the next action depends on the result of the previous one. After the action is completed (added to `question_finished` class), its success is checked to determine the required transition.

There are 3 transition options:

1. transition on successful completion of the action. Set by the relation nrel_then. The transition along this relation is carried out upon successful completion of the action from which the transition is made (its addition to the question_finished_successfully class);
2. transition on unsuccessful completion of the action. Set by the relation nrel_then. The transition along this relation is carried out upon successful completion of the action from which the transition is performed (its addition to the question_finished_unsuccessfully class);
3. unconditional transition.

Transition by action result example:

<img src="../images/non_atomic_action_interpretation_agent_transition_by_action_result_example.png"></img>

#### Conditioanl transition

In addition to transitions depending on the result of the previous action, conditional transitions are introduced through the `nrel_condition` relation. The first element of the pairs of this relation are the pairs (arcs) of transitions according to the success of the action, the second is the logical formula. In this case, an additional condition is imposed on the transition pair (in addition to the success / failure of the completion of the action) - the truth of the logical formula.

In this case, the truth of the formula is calculated for the same substitutions that were used when generating the process according to the program.

Transition conditioanl transition example:

<img src="../images/non_atomic_action_interpretation_agent_conditioanl_transition_example.png"></img>

#### Transitions priority

Transitions by pairs of nrel_then relation are checked first. If the transition was not executed (because of the unsuccessful completion of the action, the presence (and falsity) of an additional condition, or the absence of such transitions), then a similar check is performed for transitions with respect to nrel_else, then - nrel_goto. If there are several transitions, belonging to the same relation the sequence of checking such transitions is random (it depends on the state of the system memory).

If it is necessary to set a different order for checking the transition conditions, the relation nrel_basic_sequence can be used. In this case, the pair of the first transition in the sequence (of the highest priority) additionally belongs to the relation `nrel_priority_path`. In this case, the transitions in this sequence will be checked first, and only then the rest. The order for checking transitions that is in sequence is the same as described in the previous paragraph.

#### Examples

<img src="../images/non_atomic_action_interpretation_agent_transitions_priority_example_1.png"></img>

In this case, there are 2 options for the sequence of checks. First option:

1. **action_1 -> action_2 (by pair of nrel_then releation)**;
2. **action_1 -> action_3 (by pair of nrel_then releation)**;
3. action_1 -> action_4 (by pair of nrel_else releation); 
4. action_1 -> action_5 (by pair of nrel_goto releation).

Second one:

1. **action_1 -> action_3** (by pair of nrel_then releation);
2. **action_1 -> action_2** (by pair of nrel_then releation); 
3. action_1 -> action_4 (by pair of nrel_else releation);
4. action_1 -> action_5 (by pair of nrel_goto releation).


<img src="../images/non_atomic_action_interpretation_agent_transitions_priority_example_2.png"></img>

In this case, the transition check will be performed in the following sequence:

1. action_1 -> action_2 (by pair of nrel_then releation);
2. **action_1 -> action_3 (by pair of nrel_else releation)**;
3. **action_1 -> action_4 (by pair of nrel_then releation)**; 
4. action_1 -> action_5 (by pair of nrel_elsereleation).

### Result

Possible result codes:
 
* `sc_result_ok`- interpretation completed;
* `sc_result_error`- internal error.
