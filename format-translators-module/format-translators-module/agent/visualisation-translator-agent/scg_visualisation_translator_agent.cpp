#include "scg_visualisation_translator_agent.hpp"

#include "keynodes/format_translators_keynodes.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "constants/format_translators_constants.hpp"

#include "model/node.hpp"
#include "model/connector.hpp"

#include "parser/structure_to_triples_parser.hpp"
#include "parser/triples_to_model_parser.hpp"

#include "visitor/gwf_translator_visitor.hpp"

namespace formatTranslators
{
ScAddr SCgVisualisationTranslatorAgent::GetActionClass() const
{
  return FormatTranslatorsKeynodes::action_visualise_to_scg;
}

bool SCgVisualisationTranslatorAgent::CheckInitiationCondition(ScActionInitiatedEvent const & event)
{
  return m_context.CheckConnector(GetActionClass(), event.GetOtherElement(), ScType::ConstPermPosArc);
}

ScResult SCgVisualisationTranslatorAgent::DoProgram(ScAction & action)
{
  try
  {
    ScAddr const & structureToTranslate = action.GetArgument(1);
    ScAddr const & identifiersLanguage = action.GetArgument(2, FormatTranslatorsConstants::GetDefaultLanguage());
    if (!m_context.IsElement(structureToTranslate))
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Cannot find structure to translate");
    ScAddr const & keyElementsOrderTuple = utils::IteratorUtils::getAnyByOutRelation(
        &m_context, structureToTranslate, FormatTranslatorsKeynodes::nrel_key_elements_order);
    if (!m_context.IsElement(keyElementsOrderTuple))
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Cannot find key elements order for structure to translate");
    ScAddr mainKeyElement = utils::IteratorUtils::getAnyByOutRelation(
        &m_context, structureToTranslate, FormatTranslatorsKeynodes::rrel_main_key_sc_element);
    if (!m_context.IsElement(mainKeyElement))
    {
      SC_LOG_WARNING("Cannot find main key element for structure to translate, choosing random");
      auto const & elementsIterator = m_context.CreateIterator5(
          ScType::Node, ScType::Connector, ScType::Node, ScType::ConstPermPosArc, structureToTranslate);
      if (elementsIterator->Next())
        mainKeyElement = elementsIterator->Get(0);
    }
    if (!m_context.IsElement(mainKeyElement))
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Cannot find main key element for structure to translate");
    ParseKeyElementsOrder(keyElementsOrderTuple);
    StructureToTriplesParser structureToTriplesParser;
    std::shared_ptr<Triples> structureTriples = std::make_shared<Triples>();
    structureToTriplesParser.ParseStructure(
        &m_context, structureToTranslate, keyElementsOrder, identifiersLanguage, structureTriples);
    TriplesToModelParser triplesToModelParser;
    auto const & rootElement = triplesToModelParser.ParseTriples(
        &m_context, mainKeyElement, keyElementsOrder, identifiersLanguage, structureTriples);
    auto const & treeRoots = AssignXCoordinates(rootElement);
    GWFTranslatorVisitor visitor;
    lastAssignedY = 0;
    for (auto const & tree : treeRoots)
    {
      AssignYCoordinates(tree);
      tree->AcceptVisitor(visitor);
    }
    ScAddr const & resultLink = m_context.GenerateLink(ScType::ConstNodeLink);
    m_context.SetLinkContent(
        resultLink,
        FormatTranslatorsConstants::GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT + visitor.GetStaticSectorContent()
            + FormatTranslatorsConstants::GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT);
    action.FormResult(resultLink);
    return action.FinishSuccessfully();
  }
  catch (utils::ScException const & exception)
  {
    m_logger.Error(exception.Description());
    return action.FinishWithError();
  }
}

void SCgVisualisationTranslatorAgent::ParseKeyElementsOrder(ScAddr const & keyElementsOrderTuple)
{
  auto const & firstElementIterator = m_context.CreateIterator5(
      keyElementsOrderTuple, ScType::ConstPermPosArc, ScType::Node, ScType::ConstPermPosArc, ScKeynodes::rrel_1);
  if (firstElementIterator->Next())
  {
    ScAddrUnorderedSet visited;
    ScAddr arcToElement = firstElementIterator->Get(1);
    while (m_context.IsElement(arcToElement))
    {
      if (!m_context.GetElementType(arcToElement).IsMembershipArc())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Ordered set is not formed correct, arc with hash `"
                << arcToElement.Hash() << "` to sc-element of set has type "
                << std::string(m_context.GetElementType(arcToElement)) << " instead of membership arc.");
      if (visited.count(arcToElement))
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Ordered set with hash `" << keyElementsOrderTuple.Hash()
                                      << "` contains loop that starts with arc with hash `" << arcToElement.Hash()
                                      << "`.");
      ScAddr const & element = m_context.GetArcTargetElement(arcToElement);
      if (!m_context.GetElementType(element).IsNode())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Key elements order has element with hash `" << element.Hash() << "` and type "
                                                         << std::string(m_context.GetElementType(element))
                                                         << " instead of node.");
      visited.insert(arcToElement);
      keyElementsOrder[element] = keyElementsOrder.size();
      arcToElement =
          utils::IteratorUtils::getAnyByOutRelation(&m_context, arcToElement, ScKeynodes::nrel_basic_sequence);
    }
  }
}

std::list<std::shared_ptr<Node>> SCgVisualisationTranslatorAgent::AssignXCoordinates(
    std::shared_ptr<Node> const & rootElement)
{
  float nextLevelIndent = FormatTranslatorsConstants::MIN_X;
  rootElement->SetX(nextLevelIndent);
  auto treeRoots = std::list{rootElement};

  for (auto const & treeRoot : treeRoots)
    AssignXCoordinates(treeRoots, treeRoot, FormatTranslatorsConstants::MIN_X);
  return treeRoots;
}

void SCgVisualisationTranslatorAgent::AssignXCoordinates(
    std::list<std::shared_ptr<Node>> & roots,
    std::shared_ptr<Node> const & treeRoot,
    float rootElementIndent)
{
  float previousLevelMaxLinkContentSize = treeRoot->GetContent().size();
  if (previousLevelMaxLinkContentSize > FormatTranslatorsConstants::MAX_LINK_WIDTH)
    previousLevelMaxLinkContentSize = FormatTranslatorsConstants::MAX_LINK_WIDTH;
  treeRoot->SetX(rootElementIndent);

  auto treeRootIdentifierSize =
      treeRoot->HasBus() ? 0
                         : treeRoot->GetIdentifier().size() * FormatTranslatorsConstants::IDENTIFIER_CHARACTER_WIDTH;
  if (treeRootIdentifierSize < FormatTranslatorsConstants::INDENT_FOR_NODE_WITHOUT_IDENTIFIER)
    treeRootIdentifierSize = FormatTranslatorsConstants::INDENT_FOR_NODE_WITHOUT_IDENTIFIER;

  for (auto const & treeRootConnector : treeRoot->GetConnectors())
  {
    auto const & otherElement = treeRootConnector->GetOtherElement();
    float otherElementContentSize = otherElement->GetContent().size();
    if (otherElementContentSize > FormatTranslatorsConstants::MAX_LINK_WIDTH)
      otherElementContentSize = FormatTranslatorsConstants::MAX_LINK_WIDTH;
    auto deltaForNextLevel = std::max(previousLevelMaxLinkContentSize, otherElementContentSize);
    auto const treeRootConnectorIdentifierLength =
        treeRootConnector->GetIdentifier().size() / FormatTranslatorsConstants::CONNECTOR_IDENTIFIER_SCALE;
    if (treeRootConnectorIdentifierLength > deltaForNextLevel)
      deltaForNextLevel = treeRootConnectorIdentifierLength;
    for (auto const & treeRootConnectorConnector : treeRootConnector->GetConnectors())
    {
      auto const treeRootConnectorConnectorIdentifierLength =
          treeRootConnectorConnector->GetIdentifier().size()
          / FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT;
      if (treeRootConnectorConnectorIdentifierLength > deltaForNextLevel)
        deltaForNextLevel = treeRootConnectorConnectorIdentifierLength;
      auto const treeRootConnectorConnectorOtherElementIdentifierLength =
          treeRootConnectorConnector->GetOtherElement()->GetIdentifier().size()
          / FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT;
      if (treeRootConnectorConnectorOtherElementIdentifierLength > deltaForNextLevel)
        deltaForNextLevel = treeRootConnectorConnectorOtherElementIdentifierLength;
    }

    deltaForNextLevel = (deltaForNextLevel + FormatTranslatorsConstants::EMPTY_SPACE_AFTER_IDENTIFIER)
                        * FormatTranslatorsConstants::IDENTIFIER_CHARACTER_WIDTH;

    if (treeRootIdentifierSize > deltaForNextLevel)
      deltaForNextLevel = treeRootIdentifierSize;

    float const relationIndent =
        rootElementIndent + deltaForNextLevel * FormatTranslatorsConstants::REVERSED_CONNECTOR_INCIDENT_POINT_PERCENT;

    for (auto const & levelElementConnectorConnector : treeRootConnector->GetConnectors())
    {
      levelElementConnectorConnector->SetOtherElementX(relationIndent);
      if (treeRootConnector->GetIsReversed())
        levelElementConnectorConnector->SetBaseElementBalance(FormatTranslatorsConstants::NEGATIVE_BALANCE);
      else
        levelElementConnectorConnector->SetBaseElementBalance(FormatTranslatorsConstants::NEGATIVE_BALANCE);
    }
    auto otherElementIndent = rootElementIndent + deltaForNextLevel;
    if (otherElementIndent > FormatTranslatorsConstants::MAX_X)
    {
      if (!otherElement->GetConnectors().empty())
      {
        roots.push_back(otherElement);
        auto copyOfNextLevelElement = otherElement->Copy();
        treeRootConnector->SetOtherElement(copyOfNextLevelElement);
      }
      otherElement->SetX(otherElementIndent);
    }
    else
    {
      AssignXCoordinates(roots, otherElement, otherElementIndent);
    }
  }
}

void SCgVisualisationTranslatorAgent::AssignYCoordinates(std::shared_ptr<Node> const & rootElement)
{
  // TODO(kilativ-dotcom): take into account content size to move whole row down
  rootElement->SetTopY(lastAssignedY);
  if (rootElement->HasBus())
    lastAssignedY += FormatTranslatorsConstants::Y_INCREMENT;
  for (auto const & connector : rootElement->GetConnectors())
  {
    float const relationElementY = lastAssignedY - FormatTranslatorsConstants::HALF_Y_INCREMENT;
    for (auto const & relationConnector : connector->GetConnectors())
    {
      // TODO(kilativ-dotcom): if `connector` has multiple relations like scp operator then make custom placing and
      // probably move whole row down
      auto relationOtherElement = relationConnector->GetOtherElement();
      relationOtherElement->SetTopY(relationElementY);
      relationOtherElement->SetBottomY(relationElementY);
    }
    auto otherElement = connector->GetOtherElement();
    AssignYCoordinates(otherElement);
    connector->SetBaseElementBalance(FormatTranslatorsConstants::NEGATIVE_BALANCE);
  }

  rootElement->SetBottomY(
      rootElement->HasBus() ? lastAssignedY - FormatTranslatorsConstants::Y_INCREMENT : lastAssignedY);
  if (rootElement->GetConnectors().empty())
    lastAssignedY += FormatTranslatorsConstants::Y_INCREMENT;
}
}  // namespace formatTranslators
