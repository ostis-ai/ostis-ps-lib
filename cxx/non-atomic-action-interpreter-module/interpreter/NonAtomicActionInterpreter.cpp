#include <algorithm>

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-memory/sc_wait.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"

#include "constants/NonAtomicActionInterpreterConstants.hpp"

#include "exception/ActionCancelledException.hpp"

#include "keynodes/Keynodes.hpp"

#include "manager/LogicFormulaManager.hpp"

#include "utils/TemplateParamsUtils.hpp"

#include "NonAtomicActionInterpreter.hpp"

using namespace nonAtomicActionInterpreterModule;

NonAtomicActionInterpreter::NonAtomicActionInterpreter(ScMemoryContext * context)
  : context(context)
{
}

void NonAtomicActionInterpreter::interpret(
    ScAddr const & nonAtomicActionAddr,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements,
    ScAddr const & generalAction)
{
  ScAddr decompositionTuple =
      utils::IteratorUtils::getAnyByInRelation(context, nonAtomicActionAddr, Keynodes::nrel_decomposition_of_action);
  ScAddr action = getFirstSubAction(decompositionTuple);
  while (action.IsValid())
  {
    if (context->HelperCheckEdge(decompositionTuple, action, ScType::EdgeAccessConstPosPerm) == SC_FALSE)
      SC_THROW_EXCEPTION(
          utils::ExceptionCritical,
          "NonAtomicActionInterpreter: action is not belongs to nonatomic action decomposition.");

    if (context->HelperCheckEdge(Keynodes::action_cancelled, generalAction, ScType::EdgeAccessConstPosPerm))
      SC_THROW_EXCEPTION(
          commonModule::ActionCancelledException,
          "NonAtomicActionInterpreter: the processing action of the current message has been interrupted.");

    applyAction(action);
    action = getNextAction(action, replacements);
  }
}

ScAddr NonAtomicActionInterpreter::getFirstSubAction(ScAddr const & decompositionTuple)
{
  ScAddr firstAction =
      utils::IteratorUtils::getAnyByOutRelation(context, decompositionTuple, scAgentsCommon::CoreKeynodes::rrel_1);
  if (!firstAction.IsValid())
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionCritical, "Non atomic action structure is incorrect. Failed to find first action.");
  }
  return firstAction;
}

void NonAtomicActionInterpreter::applyAction(ScAddr const & actionAddr)
{
  SC_LOG_DEBUG("NonAtomicActionInterpreter: waiting for atomic action finish.");
  if (!utils::AgentUtils::applyAction(
          context, actionAddr, NonAtomicActionInterpreterConstants::INTERPRETER_ACTION_WAIT_TIME))
  {
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "NonAtomicActionInterpreter: action wait time expired.");
  }
  SC_LOG_DEBUG("NonAtomicActionInterpreter: atomic action finished.");
}

ScAddr NonAtomicActionInterpreter::getNextAction(
    ScAddr const & actionAddr,
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  ScAddrList orderedTransitionCandidates = getOrderedTransitionCandidates(actionAddr);

  ScAddr nextAction;
  for (auto const & transitionArc : orderedTransitionCandidates)
  {
    if (checkTransitionCondition(transitionArc, replacements))
    {
      nextAction = context->GetEdgeTarget(transitionArc);
      break;
    }
  }
  return nextAction;
}

ScAddrList NonAtomicActionInterpreter::getOrderedTransitionCandidates(ScAddr const & actionAddr)
{
  bool actionIsSuccessful = false;
  bool actionIsUnsuccessful = false;
  if (context->HelperCheckEdge(
          scAgentsCommon::CoreKeynodes::question_finished_successfully, actionAddr, ScType::EdgeAccessConstPosPerm))
  {
    SC_LOG_DEBUG("NonAtomicActionInterpreter: atomic action finished successfully.");
    actionIsSuccessful = true;
  }
  else if (context->HelperCheckEdge(
               scAgentsCommon::CoreKeynodes::question_finished_unsuccessfully,
               actionAddr,
               ScType::EdgeAccessConstPosPerm))
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
        actionIsSuccessful
        && context->HelperCheckEdge(Keynodes::nrel_then, transitionArc, ScType::EdgeAccessConstPosPerm);
    bool const unsuccessCaseTransitionPossibility =
        actionIsUnsuccessful
        && context->HelperCheckEdge(Keynodes::nrel_else, transitionArc, ScType::EdgeAccessConstPosPerm);
    bool const unconditionalTransitionPossibility =
        context->HelperCheckEdge(Keynodes::nrel_goto, transitionArc, ScType::EdgeAccessConstPosPerm);

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

  ScIterator5Ptr priorityArcIterator = context->Iterator5(
      node, ScType::EdgeDCommonConst, ScType::NodeConst, ScType::EdgeAccessConstPosPerm, Keynodes::nrel_priority_path);
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
  bool result;

  ScAddr logicFormula = utils::IteratorUtils::getAnyByOutRelation(context, transitionArc, Keynodes::nrel_condition);
  if (!logicFormula.IsValid())
    return true;

  static LogicFormulaManager logicFormulaSearcher;
  return logicFormulaSearcher.checkLogicalFormula(context, logicFormula, replacements);
}
