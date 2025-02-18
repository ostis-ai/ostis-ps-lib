#include "structure_translator_agent.hpp"

#include "keynodes/format_translators_keynodes.hpp"

namespace formatTranslators
{
ScAddr StructureTranslatorAgent::GetEventSubscriptionElement() const
{
  return FormatTranslatorsKeynodes::ui_command_initiated;
}

ScAddr StructureTranslatorAgent::GetActionClass() const
{
  return FormatTranslatorsKeynodes::ui_command_translate_from_sc;
}

bool StructureTranslatorAgent::CheckInitiationCondition(ScActionInitiatedEvent const & event)
{
  ScAddr const & command = event.GetOtherElement();
  if (!m_context.CheckConnector(GetActionClass(), command, ScType::ConstPermPosArc))
    return false;
  auto const & formatIterator = m_context.CreateIterator5(
      command,
      ScType::ConstPermPosArc,
      FormatTranslatorsKeynodes::format_scg_json,
      ScType::ConstPermPosArc,
      FormatTranslatorsKeynodes::ui_rrel_output_format);
  if (!formatIterator->Next())
    return false;
  auto const & structureToTranslateIterator = m_context.CreateIterator5(
      command,
      ScType::ConstPermPosArc,
      ScType::ConstNode,
      ScType::ConstPermPosArc,
      FormatTranslatorsKeynodes::ui_rrel_source_sc_construction);
  if (!structureToTranslateIterator->Next())
    return false;
  structureToTranslate = structureToTranslateIterator->Get(2);
  return m_context.IsElement(structureToTranslate);
}

ScResult StructureTranslatorAgent::DoProgram(
    ScEventAfterGenerateOutgoingArc<ScType::ConstPermPosArc> const & event,
    ScAction & fakeAction)
{
  try
  {
    ScAction action = m_context.ConvertToAction(event.GetOtherElement());
    if (!m_context.IsElement(structureToTranslate))
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Cannot find structure to translate");
    ScAddr const & identifiersLanguage =
        action.GetArgument(FormatTranslatorsKeynodes::ui_rrel_user_lang, FormatTranslatorsKeynodes::lang_en);

    ScAction keyElementsOrderAction = m_context.GenerateAction(FormatTranslatorsKeynodes::action_order_key_elements);
    keyElementsOrderAction.SetArguments(structureToTranslate);
    keyElementsOrderAction.InitiateAndWait(1000);

    ScAction scgVisualiseAction = m_context.GenerateAction(FormatTranslatorsKeynodes::action_visualise_to_scg);
    scgVisualiseAction.SetArguments(structureToTranslate, identifiersLanguage);
    scgVisualiseAction.InitiateAndWait(300000);
    ScAddr const & scgVisualiseActionResult = scgVisualiseAction.GetResult();
    auto const & it1 =
        m_context.CreateIterator3(scgVisualiseActionResult, ScType::ConstPermPosArc, ScType::ConstNodeLink);
    ScAddr const & resultLink = it1->Next() ? it1->Get(2) : m_context.GenerateLink(ScType::ConstNodeLink);

    ScAddr arc =
        m_context.GenerateConnector(ScType::ConstCommonArc, resultLink, FormatTranslatorsKeynodes::format_scg_json);
    m_context.GenerateConnector(ScType::ConstPermPosArc, FormatTranslatorsKeynodes::nrel_format, arc);
    arc = m_context.GenerateConnector(ScType::ConstCommonArc, structureToTranslate, resultLink);
    m_context.GenerateConnector(ScType::ConstPermPosArc, FormatTranslatorsKeynodes::nrel_translation, arc);

    fakeAction.SetResult(resultLink);
    return fakeAction.FinishSuccessfully();
  }
  catch (utils::ScException const & exception)
  {
    m_logger.Error(exception.Description());
    return fakeAction.FinishWithError();
  }
}
}  // namespace formatTranslators
