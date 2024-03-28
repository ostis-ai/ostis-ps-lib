#pragma once

#include "sc-memory/sc_memory.hpp"

namespace nonAtomicActionInterpreterModule
{
class NonAtomicActionInterpreter
{
public:
  explicit NonAtomicActionInterpreter(ScMemoryContext * context);

  void interpret(
      ScAddr const & nonAtomicActionAddr,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers,
      ScAddr const & generalAction);

private:
  ScMemoryContext * context;

  ScAddr getFirstSubAction(ScAddr const & decompositionTuple);

  void applyAction(ScAddr const & actionAddr);

  ScAddr getNextAction(
      ScAddr const & previousActionAddr,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers);

  ScAddrVector getOrderedTransitionCandidates(ScAddr const & actionAddr);

  ScAddrVector getOrderedTransitionCandidatesFromSequence(
      ScAddr const & actionAddr,
      bool const & actionIsSuccessful,
      bool const & actionIsUnsuccessful);

  void updateTransitionCandidatesWithTheCandidatesNotFromSequence(
      ScAddrVector & orderedTransitionCandidates,
      ScAddr const & actionAddr,
      bool const & actionIsSuccessful,
      bool const & actionIsUnsuccessful);

  void updateTransitionsByRelation(ScAddr const & action, ScAddr const & relation, ScAddrVector & transitions);

  ScAddr getPriorityTransitionArc(ScAddr const & node);

  ScAddrVector getAllArcsByOutRelation(ScAddr const & node, ScAddr const & relation);

  bool checkTransitionCondition(
      ScAddr const & transitionArc,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers);
};

}  // namespace nonAtomicActionInterpreterModule
