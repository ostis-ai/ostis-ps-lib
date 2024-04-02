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

  ScAddrList getOrderedTransitionCandidates(ScAddr const & actionAddr);

  ScAddrList getOrderedTransitionCandidatesFromSequence(
      ScAddr const & actionAddr,
      bool const & actionIsSuccessful,
      bool const & actionIsUnsuccessful);

  void updateTransitionCandidatesWithTheCandidatesNotFromSequence(
      ScAddrList & orderedTransitionCandidates,
      ScAddr const & actionAddr,
      bool const & actionIsSuccessful,
      bool const & actionIsUnsuccessful);

  void updateTransitionsByRelation(ScAddr const & action, ScAddr const & relation, ScAddrList & transitions);

  ScAddr getPriorityTransitionArc(ScAddr const & node);

  ScAddrList getAllArcsByOutRelation(ScAddr const & node, ScAddr const & relation);

  bool checkTransitionCondition(
      ScAddr const & transitionArc,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      std::map<ScAddr, std::string, ScAddrLessFunc> const & resolvedVariableIdentifiers);
};

}  // namespace nonAtomicActionInterpreterModule
