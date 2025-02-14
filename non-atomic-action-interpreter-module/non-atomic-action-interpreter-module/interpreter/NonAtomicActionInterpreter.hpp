#pragma once

#include <sc-memory/sc_action.hpp>

namespace nonAtomicActionInterpreterModule
{
class NonAtomicActionInterpreter
{
public:
  explicit NonAtomicActionInterpreter(ScAgentContext * context);

  void interpret(
      ScAddr const & nonAtomicActionAddr,
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
      ScAddr const & generalAction);

private:
  ScAgentContext * context;

  ScAction getFirstSubAction(ScAddr const & decompositionTuple);

  static void applyAction(ScAction & actionAddr);

  bool getNextAction(ScAction & actionAddr, std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);

  ScAddrList getOrderedTransitionCandidates(ScAction const & actionAddr);

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
      std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements);
};

}  // namespace nonAtomicActionInterpreterModule
