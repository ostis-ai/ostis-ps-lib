#include "NonAtomicActionInterpreterAgent.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <common/action_cancelled_exception.hpp>
#include <common/utils/template_params_utils.hpp>
#include <common/utils/macros.hpp>

#include "keynodes/NonAtomicKeynodes.hpp"

using namespace nonAtomicActionInterpreterModule;

ScResult NonAtomicActionInterpreterAgent::DoProgram(ScActionInitiatedEvent const & event, ScAction & action)
{
  START_TIMER();

  ScAddr nonAtomicActionAddr;
  std::map<ScAddr, ScAddr, ScAddrLessFunc> replacements;
  ScAddr generalAction;
  try
  {
    ScAddr const & nonAtomicActionTemplateAddr = action.GetArgument(1);

    SC_CHECK_PARAM(nonAtomicActionTemplateAddr, "action params are not formed correctly.");

    ScAddr const & argumentsSet = action.GetArgument(2);
    replacements = createReplacements(nonAtomicActionTemplateAddr, argumentsSet);
    validateVariableIdentifiers(replacements);

    ScTemplateParams programTemplateParams =
        common::TemplateParamsUtils::CreateTemplateParamsFromReplacements(replacements);
    nonAtomicActionAddr = replaceNonAtomicAction(nonAtomicActionTemplateAddr, programTemplateParams);
    generateNonAtomicActionTemplate(nonAtomicActionTemplateAddr, programTemplateParams);

    generalAction = utils::IteratorUtils::getAnyByInRelation(&m_context, action, Keynodes::nrel_subaction);

    initFields();
    nonAtomicActionInterpreter->interpret(nonAtomicActionAddr, replacements, generalAction);
  }
  catch (common::ActionCancelledException const & exception)
  {
    m_logger.Error(exception.Description());
    m_context.GenerateConnector(ScType::ConstPermPosArc, Keynodes::action_cancelled, action);
    STOP_TIMER("NonAtomicActionInterpreterAgent");
    return action.FinishUnsuccessfully();
  }
  catch (utils::ScException & ex)
  {
    m_logger.Error(ex.Message());
    STOP_TIMER("NonAtomicActionInterpreterAgent");
    return action.FinishUnsuccessfully();
  }

  STOP_TIMER("NonAtomicActionInterpreterAgent");
  return action.FinishSuccessfully();
}

ScAddr NonAtomicActionInterpreterAgent::GetActionClass() const
{
  return Keynodes::action_interpret_non_atomic_action;
}

void NonAtomicActionInterpreterAgent::generateNonAtomicActionTemplate(
    ScAddr const & nonAtomicActionTemplateAddr,
    ScTemplateParams const & templateParams)
{
  ScTemplate nonAtomicActionTemplate;
  m_context.BuildTemplate(nonAtomicActionTemplate, nonAtomicActionTemplateAddr);
  ScTemplateGenResult templateGenResult;
  m_context.GenerateByTemplate(nonAtomicActionTemplate, templateGenResult, templateParams);
}

ScAddr NonAtomicActionInterpreterAgent::getTemplateKeyElement(ScAddr const & templateAddr)
{
  ScAddr templateKeyElement =
      utils::IteratorUtils::getAnyByOutRelation(&m_context, templateAddr, ScKeynodes::rrel_key_sc_element);

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
      ScAddr role = utils::IteratorUtils::getRoleRelation(&m_context, index);
      if (!role.IsValid())
      {
        break;
      }
      ScAddr argument = utils::IteratorUtils::getAnyByOutRelation(&m_context, argumentsSet, role);
      if (!argument.IsValid())
      {
        break;
      }
      ScIterator5Ptr variablesIterator5 = m_context.CreateIterator5(
          templateKeyElement, ScType::VarPermPosArc, ScType::VarNode, ScType::VarPermPosArc, role);
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
    std::string identifier = m_context.GetElementSystemIdentifier(varAddr);
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
  ScAddr keyElementReplacement = m_context.GenerateNode(ScType::ConstNode);
  ScAddr templateKeyElement;
  templateKeyElement = getTemplateKeyElement(templateAddr);
  templateParams.Add(m_context.GetElementSystemIdentifier(templateKeyElement), keyElementReplacement);

  return keyElementReplacement;
}

void NonAtomicActionInterpreterAgent::initFields()
{
  this->nonAtomicActionInterpreter = std::make_unique<NonAtomicActionInterpreter>(&m_context);
}
