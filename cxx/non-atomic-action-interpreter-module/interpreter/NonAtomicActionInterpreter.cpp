#include <algorithm>

#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "constants/NonAtomicActionInterpreterConstants.hpp"

#include "exception/ActionCancelledException.hpp"

#include "keynodes/NonAtomicKeynodes.hpp"

#include "utils/LogicUtils.hpp"
#include "utils/TemplateParamsUtils.hpp"

#include "NonAtomicActionInterpreter.hpp"

using namespace nonAtomicActionInterpreterModule;

NonAtomicActionInterpreter::NonAtomicActionInterpreter(ScAgentContext * context)
  : context(context)
{
}

void NonAtomicActionInterpreter::interpret(
    ScAddr const & nonAtomicActionAddr,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    ScAddr const & generalAction)
{
  ScAddr decompositionTuple =
      utils::IteratorUtils::getAnyByOutRelation(context, nonAtomicActionAddr, Keynodes::nrel_decomposition_of_action);
  ScAction action = getFirstSubAction(decompositionTuple);
  bool hasActionToApply = context->IsElement(action);
  while (hasActionToApply)
  {
    if (context->CheckConnector(decompositionTuple, action, ScType::ConstPermPosArc) == SC_FALSE)
      SC_THROW_EXCEPTION(
          utils::ExceptionCritical,
          "NonAtomicActionInterpreter: action is not belongs to non-atomic action decomposition.");

    if (context->CheckConnector(Keynodes::action_cancelled, generalAction, ScType::ConstPermPosArc))
      SC_THROW_EXCEPTION(
          commonModule::ActionCancelledException,
          "NonAtomicActionInterpreter: the processing action of the current non-atomic action has been interrupted.");

    applyAction(action);
    hasActionToApply = getNextAction(action, replacements);
  }
}

ScAction NonAtomicActionInterpreter::getFirstSubAction(ScAddr const & decompositionTuple)
{
  ScAddr firstAction = utils::IteratorUtils::getAnyByOutRelation(context, decompositionTuple, ScKeynodes::rrel_1);
  if (!firstAction.IsValid())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "Non-atomic action structure is incorrect. Failed to find first action.");
  }
  return context->ConvertToAction(firstAction);
}

void NonAtomicActionInterpreter::applyAction(ScAction & actionAddr)
{
  SC_LOG_DEBUG("NonAtomicActionInterpreter: waiting for atomic action finish.");
  if (!actionAddr.InitiateAndWait(NonAtomicActionInterpreterConstants::INTERPRETER_ACTION_WAIT_TIME))
  {
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "NonAtomicActionInterpreter: action wait time expired.");
  }
  SC_LOG_DEBUG("NonAtomicActionInterpreter: atomic action finished.");
}

bool NonAtomicActionInterpreter::getNextAction(
    ScAction & actionAddr,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  ScAddrList orderedTransitionCandidates = getOrderedTransitionCandidates(actionAddr);

  ScAddr nextAction;
  for (auto const & transitionArc : orderedTransitionCandidates)
  {
    if (checkTransitionCondition(transitionArc, replacements))
    {
      nextAction = context->GetArcTargetElement(transitionArc);
      actionAddr = context->ConvertToAction(nextAction);
      return true;
    }
  }
  return false;
}

ScAddrList NonAtomicActionInterpreter::getOrderedTransitionCandidates(ScAction const & actionAddr)
{
  bool actionIsSuccessful = false;
  bool actionIsUnsuccessful = false;
  if (actionAddr.IsFinishedSuccessfully())
  {
    SC_LOG_DEBUG("NonAtomicActionInterpreter: atomic action finished successfully.");
    actionIsSuccessful = true;
  }
  else if (actionAddr.IsFinishedUnsuccessfully())
  {
    SC_LOG_DEBUG("NonAtomicActionInterpreter: atomic action finished unsuccessfully.");
    actionIsUnsuccessful = true;
  }
  else
    SC_LOG_DEBUG("NonAtomicActionInterpreter: atomic action finished with unknown result.");

  ScAddrList orderedTransitionCandidates =
      getOrderedTransitionCandidatesFromSequence(actionAddr, actionIsSuccessful, actionIsUnsuccessful);

  updateTransitionCandidatesWithTheCandidatesNotFromSequence(
      orderedTransitionCandidates, actionAddr, actionIsSuccessful, actionIsUnsuccessful);

  return orderedTransitionCandidates;
}

ScAddrList NonAtomicActionInterpreter::getOrderedTransitionCandidatesFromSequence(
    ScAddr const & actionAddr,
    bool const & actionIsSuccessful,
    bool const & actionIsUnsuccessful)
{
  ScAddrList orderedTransitionCandidates;

  ScAddr transitionArc = getPriorityTransitionArc(actionAddr);
  while (transitionArc.IsValid())
  {
    bool const successCaseTransitionPossibility =
        actionIsSuccessful && context->CheckConnector(Keynodes::nrel_then, transitionArc, ScType::ConstPermPosArc);
    bool const unsuccessCaseTransitionPossibility =
        actionIsUnsuccessful && context->CheckConnector(Keynodes::nrel_else, transitionArc, ScType::ConstPermPosArc);
    bool const unconditionalTransitionPossibility =
        context->CheckConnector(Keynodes::nrel_goto, transitionArc, ScType::ConstPermPosArc);

    if (successCaseTransitionPossibility || unsuccessCaseTransitionPossibility || unconditionalTransitionPossibility)
      orderedTransitionCandidates.push_back(transitionArc);

    transitionArc = utils::IteratorUtils::getAnyByOutRelation(context, transitionArc, Keynodes::nrel_basic_sequence);
  }

  return orderedTransitionCandidates;
}

void NonAtomicActionInterpreter::updateTransitionCandidatesWithTheCandidatesNotFromSequence(
    ScAddrList & orderedTransitionCandidates,
    ScAddr const & actionAddr,
    bool const & actionIsSuccessful,
    bool const & actionIsUnsuccessful)
{
  if (actionIsSuccessful)
  {
    updateTransitionsByRelation(actionAddr, Keynodes::nrel_then, orderedTransitionCandidates);
  }
  else if (actionIsUnsuccessful)
  {
    updateTransitionsByRelation(actionAddr, Keynodes::nrel_else, orderedTransitionCandidates);
  }

  updateTransitionsByRelation(actionAddr, Keynodes::nrel_goto, orderedTransitionCandidates);
}

void NonAtomicActionInterpreter::updateTransitionsByRelation(
    ScAddr const & action,
    ScAddr const & relation,
    ScAddrList & transitions)
{
  ScAddrList transitionArcs = getAllArcsByOutRelation(action, relation);
  for (auto const & arc : transitionArcs)
  {
    if (std::find(transitions.begin(), transitions.end(), arc) == transitions.end())
      transitions.push_back(arc);
  }
}

ScAddr NonAtomicActionInterpreter::getPriorityTransitionArc(ScAddr const & node)
{
  ScAddr arcAddr;

  ScIterator5Ptr priorityArcIterator = context->CreateIterator5(
      node, ScType::ConstCommonArc, ScType::ConstNode, ScType::ConstPermPosArc, Keynodes::nrel_priority_path);
  if (priorityArcIterator->Next())
  {
    arcAddr = priorityArcIterator->Get(1);
  }
  return arcAddr;
}

ScAddrList NonAtomicActionInterpreter::getAllArcsByOutRelation(ScAddr const & node, ScAddr const & relation)
{
  ScAddrList arcs;
  ScIterator5Ptr iterator5 = utils::IteratorUtils::getIterator5(context, node, relation, true);
  while (iterator5->Next())
  {
    arcs.push_back(iterator5->Get(1));
  }
  return arcs;
}

bool NonAtomicActionInterpreter::checkTransitionCondition(
    ScAddr const & transitionArc,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  ScAddr logicFormula = utils::IteratorUtils::getAnyByOutRelation(context, transitionArc, Keynodes::nrel_condition);
  if (!logicFormula.IsValid())
    return true;

  return commonModule::LogicUtils::checkLogicalFormula(context, logicFormula, replacements);
}
