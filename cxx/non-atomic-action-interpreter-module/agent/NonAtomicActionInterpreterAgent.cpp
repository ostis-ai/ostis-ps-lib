#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/Keynodes.hpp"
#include "utils/RelationUtils.hpp"
#include "utils/TemplateParamsUtils.hpp"
#include "exception/ActionCancelledException.hpp"

#include "NonAtomicActionInterpreterAgent.hpp"

#include "utils/Macros.hpp"

using namespace nonAtomicActionInterpreterModule;
using namespace scAgentsCommon;

SC_AGENT_IMPLEMENTATION(NonAtomicActionInterpreterAgent)
{
  START_TIMER();
  ScAddr actionAddr = otherAddr;
  if (checkActionClass(actionAddr) == SC_FALSE)
    return SC_RESULT_OK;

  SC_LOG_DEBUG("NonAtomicActionInterpreterAgent started");

  ScAddr nonAtomicActionAddr;
  std::map<ScAddr, ScAddr, ScAddrLessFunc> replacements;
  ScAddr generalAction;
  try
  {
    ScAddr nonAtomicActionTemplateAddr =
        utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionAddr, CoreKeynodes::rrel_1);

    SC_CHECK_PARAM(nonAtomicActionTemplateAddr, "action params are not formed correctly.");

    ScAddr argumentsSet = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, actionAddr, CoreKeynodes::rrel_2);
    replacements = createReplacements(nonAtomicActionTemplateAddr, argumentsSet);
    validateVariableIdentifiers(replacements);

    ScTemplateParams programTemplateParams = TemplateParamsUtils::createTemplateParamsFromReplacements(replacements);
    nonAtomicActionAddr = replaceNonAtomicAction(nonAtomicActionTemplateAddr, programTemplateParams);
    generateNonAtomicActionTemplate(nonAtomicActionTemplateAddr, programTemplateParams);

    generalAction = utils::IteratorUtils::getAnyByInRelation(&m_memoryCtx, actionAddr, Keynodes::nrel_subaction);
  }
  catch (utils::ExceptionInvalidParams const & ex)
  {
    SC_LOG_ERROR("NonAtomicActionInterpreterAgent: " << ex.Message());
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, false);
    STOP_TIMER("NonAtomicActionInterpreterAgent");
    return SC_RESULT_ERROR_INVALID_PARAMS;
  }

  initFields();
  try
  {
    nonAtomicActionInterpreter->interpret(nonAtomicActionAddr, replacements, generalAction);
  }
  catch (utils::ExceptionCritical & ex)
  {
    SC_LOG_ERROR(ex.Message());
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, false);
    STOP_TIMER("NonAtomicActionInterpreterAgent");
    return SC_RESULT_ERROR;
  }
  catch (nonAtomicActionInterpreterModule::ActionCancelledException const & exception)
  {
    SC_LOG_ERROR(exception.Description());
    SC_LOG_DEBUG("NonAtomicActionInterpreterAgent finished with errors");
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::action_cancelled, actionAddr);
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, false);
    STOP_TIMER("NonAtomicActionInterpreterAgent");
    return SC_RESULT_ERROR;
  }
  SC_LOG_DEBUG("NonAtomicActionInterpreterAgent finished");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, actionAddr, true);
  STOP_TIMER("NonAtomicActionInterpreterAgent");
  return SC_RESULT_OK;
}

bool NonAtomicActionInterpreterAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
      Keynodes::action_interpret_non_atomic_action, actionAddr, ScType::EdgeAccessConstPosPerm);
}

void NonAtomicActionInterpreterAgent::generateNonAtomicActionTemplate(
    ScAddr const & nonAtomicActionTemplateAddr,
    ScTemplateParams const & templateParams)
{
  ScTemplate nonAtomicActionTemplate;
  m_memoryCtx.HelperBuildTemplate(nonAtomicActionTemplate, nonAtomicActionTemplateAddr);
  ScTemplateGenResult templateGenResult;
  if (!m_memoryCtx.HelperGenTemplate(nonAtomicActionTemplate, templateGenResult, templateParams))
  {
    SC_THROW_EXCEPTION(utils::ExceptionCritical, "template generation error.");
  }
}

ScAddr NonAtomicActionInterpreterAgent::getTemplateKeyElement(ScAddr const & templateAddr)
{
  ScAddr templateKeyElement =
      utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, templateAddr, CoreKeynodes::rrel_key_sc_element);

  if (!templateKeyElement.IsValid())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "template key element not found.");
  }

  return templateKeyElement;
}

std::map<ScAddr, ScAddr, ScAddrLessFunc> NonAtomicActionInterpreterAgent::createReplacements(
    ScAddr const & nonAtomicActionTemplate,
    ScAddr const & argumentsSet)
{
  std::map<ScAddr, ScAddr, ScAddrLessFunc> replacements;
  if (argumentsSet.IsValid())
  {
    ScAddr templateKeyElement;
    templateKeyElement = getTemplateKeyElement(nonAtomicActionTemplate);
    for (int index = 1;; index++)
    {
      ScAddr role = utils::IteratorUtils::getRoleRelation(&m_memoryCtx, index);
      if (!role.IsValid())
      {
        break;
      }
      ScAddr argument = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, argumentsSet, role);
      if (!argument.IsValid())
      {
        break;
      }
      ScIterator5Ptr variablesIterator5 = m_memoryCtx.Iterator5(
          templateKeyElement, ScType::EdgeAccessVarPosPerm, ScType::NodeVar, ScType::EdgeAccessVarPosPerm, role);
      if (variablesIterator5->Next())
      {
        replacements[variablesIterator5->Get(2)] = argument;
      }
    }
  }

  return replacements;
}

void NonAtomicActionInterpreterAgent::validateVariableIdentifiers(
    std::map<ScAddr, ScAddr, ScAddrLessFunc> const & replacements)
{
  for (auto const & [varAddr, value] : replacements)
  {
    std::string identifier = m_memoryCtx.HelperGetSystemIdtf(varAddr);
    if (identifier.empty())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "all argument variables should have identifiers.");
    }
  }
}

ScAddr NonAtomicActionInterpreterAgent::replaceNonAtomicAction(
    ScAddr const & templateAddr,
    ScTemplateParams & templateParams)
{
  ScAddr keyElementReplacement = m_memoryCtx.CreateNode(ScType::NodeConst);
  ScAddr templateKeyElement;
  templateKeyElement = getTemplateKeyElement(templateAddr);
  templateParams.Add(m_memoryCtx.HelperGetSystemIdtf(templateKeyElement), keyElementReplacement);

  return keyElementReplacement;
}

void NonAtomicActionInterpreterAgent::initFields()
{
  this->nonAtomicActionInterpreter = std::make_unique<NonAtomicActionInterpreter>(&m_memoryCtx);
}
