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
    structureToTranslate = action.GetArgument(1);
    identifiersLanguage = action.GetArgument(2, FormatTranslatorsConstants::GetDefaultLanguage());
    if (!m_context.IsElement(structureToTranslate))
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Cannot find structure to translate");
    ScAddr const & keyElementsOrderTuple = utils::IteratorUtils::getAnyByOutRelation(
        &m_context, structureToTranslate, FormatTranslatorsKeynodes::nrel_key_elements_order);
    if (!m_context.IsElement(keyElementsOrderTuple))
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Cannot find key elements order for structure to translate");
    ScAddr const & mainKeyElement = utils::IteratorUtils::getAnyByOutRelation(
        &m_context, structureToTranslate, FormatTranslatorsKeynodes::rrel_main_key_sc_element);
    if (!m_context.IsElement(mainKeyElement))
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Cannot find main key element for structure to translate");
    ParseKeyElementsOrder(keyElementsOrderTuple);
    StructureToTriplesParser structureToTriplesParser;
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
  float previousLevelMaxLinkContentSize = rootElement->GetContent().size();
  if (previousLevelMaxLinkContentSize > FormatTranslatorsConstants::MAX_LINK_WIDTH)
    previousLevelMaxLinkContentSize = FormatTranslatorsConstants::MAX_LINK_WIDTH;
  float nextLevelIndent = FormatTranslatorsConstants::MIN_X;
  rootElement->SetX(nextLevelIndent);
  auto treeRoots = std::list{rootElement};

  for (auto const & treeRoot : treeRoots)
  {
    treeRoot->SetX(nextLevelIndent);
    auto currentLevelRoots = std::list{treeRoot};
    while (!currentLevelRoots.empty())
    {
      std::list<std::shared_ptr<Connector>> connectorsToNextLevelRoots;
      std::list<std::shared_ptr<Node>> nextLevelRoots;
      float maxRelationIdentifierLength = 0;
      float maxLevelElementIdentifierLength = 0;
      CalculateNextLevelElementsAndIdentifiersLengths(
          currentLevelRoots,
          connectorsToNextLevelRoots,
          nextLevelRoots,
          maxRelationIdentifierLength,
          maxLevelElementIdentifierLength);
      float const indentCausedByLinkContent =
          CalculateIndentCausedByLinkContent(connectorsToNextLevelRoots, previousLevelMaxLinkContentSize);
      float const indentDifference =
          (std::max(maxLevelElementIdentifierLength, maxRelationIdentifierLength) + indentCausedByLinkContent
           + FormatTranslatorsConstants::EMPTY_SPACE_AFTER_IDENTIFIER)
          * FormatTranslatorsConstants::IDENTIFIER_CHARACTER_WIDTH;
      nextLevelIndent += indentDifference;
      bool const reachedEndOfIndent = nextLevelIndent >= FormatTranslatorsConstants::MAX_Y;
      if (reachedEndOfIndent)
        UpdateNextLevelRoots(treeRoots, connectorsToNextLevelRoots, nextLevelRoots);

      for (auto const & nextLevelElement : nextLevelRoots)
        nextLevelElement->SetX(nextLevelIndent);
      float const relationIndent =
          nextLevelIndent - indentDifference * FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT;
      SetRelationIndent(currentLevelRoots, relationIndent);
      if (reachedEndOfIndent)
      {
        nextLevelIndent = FormatTranslatorsConstants::MIN_X;
        currentLevelRoots.clear();
      }
      else
        currentLevelRoots = nextLevelRoots;
      nextLevelRoots.clear();
    }
  }

  return treeRoots;
}

float SCgVisualisationTranslatorAgent::CalculateIndentCausedByLinkContent(
    std::list<std::shared_ptr<Connector>> const & connectorsToNextLevelRoots,
    float & previousLevelMaxLinkContentSize)
{
  float indentCausedByLinkContent = 0;
  for (auto const & connectorToNextLevelElement : connectorsToNextLevelRoots)
  {
    auto const & nextLevelElement = connectorToNextLevelElement->GetOtherElement();
    auto const & nextLevelElementContentSize = nextLevelElement->GetContent().size();
    if (nextLevelElementContentSize > indentCausedByLinkContent)
      indentCausedByLinkContent = nextLevelElementContentSize;
  }
  auto const & maxNextLevelLinkContentSize =
      std::min(indentCausedByLinkContent, FormatTranslatorsConstants::MAX_LINK_WIDTH);
  indentCausedByLinkContent = std::max(maxNextLevelLinkContentSize, previousLevelMaxLinkContentSize);
  previousLevelMaxLinkContentSize = maxNextLevelLinkContentSize;
  return indentCausedByLinkContent;
}

void SCgVisualisationTranslatorAgent::CalculateNextLevelElementsAndIdentifiersLengths(
    std::list<std::shared_ptr<Node>> const & currentLevelRoots,
    std::list<std::shared_ptr<Connector>> & connectorsToNextLevelRoots,
    std::list<std::shared_ptr<Node>> & nextLevelRoots,
    float & maxRelationIdentifierLength,
    float & maxLevelElementIdentifierLength)
{
  // todo(kilativ-dotcom): match russian letters regex and subtract from identifier length
  for (auto const & levelElement : currentLevelRoots)
  {
    auto const & levelElementIdentifierSize = levelElement->GetIdentifier().size();
    if (levelElementIdentifierSize > maxLevelElementIdentifierLength)
      maxLevelElementIdentifierLength = levelElementIdentifierSize;

    for (auto const & levelElementConnector : levelElement->GetConnectors())
    {
      connectorsToNextLevelRoots.push_back(levelElementConnector);
      nextLevelRoots.push_back(levelElementConnector->GetOtherElement());
      auto const levelElementConnectorIdentifierLength =
          levelElementConnector->GetIdentifier().size() / FormatTranslatorsConstants::CONNECTOR_IDENTIFIER_SCALE;
      if (levelElementConnectorIdentifierLength > maxRelationIdentifierLength)
        maxRelationIdentifierLength = levelElementConnectorIdentifierLength;
      for (auto const & levelElementConnectorConnector : levelElementConnector->GetConnectors())
      {
        auto const levelElementConnectorConnectorIdentifierLength =
            levelElementConnectorConnector->GetIdentifier().size()
            / FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT;
        if (levelElementConnectorConnectorIdentifierLength > maxRelationIdentifierLength)
          maxRelationIdentifierLength = levelElementConnectorConnectorIdentifierLength;
        auto const levelElementConnectorConnectorOtherElementIdentifierLength =
            levelElementConnectorConnector->GetOtherElement()->GetIdentifier().size()
            / FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT;
        if (levelElementConnectorConnectorOtherElementIdentifierLength > maxRelationIdentifierLength)
          maxRelationIdentifierLength = levelElementConnectorConnectorOtherElementIdentifierLength;
      }
    }
  }
}

void SCgVisualisationTranslatorAgent::UpdateNextLevelRoots(
    std::list<std::shared_ptr<Node>> & treeRoots,
    std::list<std::shared_ptr<Connector>> & connectorsToNextLevelRoots,
    std::list<std::shared_ptr<Node>> & nextLevelRoots)
{
  nextLevelRoots.clear();
  std::list<std::shared_ptr<Connector>> updatedNextLevel;
  auto connectorToNextLevelElementIterator = connectorsToNextLevelRoots.begin();
  while (connectorToNextLevelElementIterator != connectorsToNextLevelRoots.end())
  {
    auto & connectorToNextLevel = *connectorToNextLevelElementIterator;
    auto const & otherElement = connectorToNextLevel->GetOtherElement();
    if (!otherElement->GetConnectors().empty())
    {
      treeRoots.push_back(otherElement);
      auto copyOfNextLevelElement = otherElement->Copy();
      connectorToNextLevelElementIterator = connectorsToNextLevelRoots.erase(connectorToNextLevelElementIterator);
      connectorToNextLevel->SetOtherElement(copyOfNextLevelElement);
      updatedNextLevel.push_back(connectorToNextLevel);
      nextLevelRoots.push_back(copyOfNextLevelElement);
    }
    else
    {
      updatedNextLevel.push_back(connectorToNextLevel);
      nextLevelRoots.push_back(connectorToNextLevel->GetOtherElement());
      ++connectorToNextLevelElementIterator;
    }
  }
  connectorsToNextLevelRoots.clear();
  connectorsToNextLevelRoots.splice(connectorsToNextLevelRoots.begin(), updatedNextLevel);
}

void SCgVisualisationTranslatorAgent::SetRelationIndent(
    std::list<std::shared_ptr<Node>> const & currentLevelRoots,
    float const relationIndent)
{
  for (auto const & levelElement : currentLevelRoots)
  {
    for (auto const & levelElementConnector : levelElement->GetConnectors())
    {
      for (auto const & levelElementConnectorConnector : levelElementConnector->GetConnectors())
      {
        levelElementConnectorConnector->SetOtherElementX(relationIndent);
        if (levelElementConnector->GetIsReversed())
          levelElementConnectorConnector->SetBaseElementBalance(
              FormatTranslatorsConstants::CONNECTOR_INCIDENT_POINT_PERCENT);
        else
          levelElementConnectorConnector->SetBaseElementBalance(
              FormatTranslatorsConstants::REVERSED_CONNECTOR_INCIDENT_POINT_PERCENT);
      }
    }
  }
}

void SCgVisualisationTranslatorAgent::AssignYCoordinates(std::shared_ptr<Node> const & rootElement)
{
  rootElement->SetTopY(lastAssignedY);
  if (rootElement->HasBus())
    lastAssignedY += FormatTranslatorsConstants::Y_INCREMENT;
  float levelElementConnectorIndex = 1;
  size_t const rootElementConnectorsSize = rootElement->CalculateReservedVerticalElementsOnParent() - 1;
  for (auto const & connector : rootElement->GetConnectors())
  {
    float const relationElementY = lastAssignedY - FormatTranslatorsConstants::HALF_Y_INCREMENT;
    for (auto const & relationConnector : connector->GetConnectors())
    {
      auto relationOtherElement = relationConnector->GetOtherElement();
      relationOtherElement->SetTopY(relationElementY);
      relationOtherElement->SetBottomY(relationElementY);
    }
    auto otherElement = connector->GetOtherElement();
    AssignYCoordinates(otherElement);
    connector->SetBaseElementBalance(levelElementConnectorIndex / rootElementConnectorsSize);
    levelElementConnectorIndex += otherElement->CalculateReservedVerticalElementsOnParent();
  }

  rootElement->SetBottomY(
      rootElement->HasBus() ? lastAssignedY - FormatTranslatorsConstants::Y_INCREMENT : lastAssignedY);
  if (rootElement->GetConnectors().empty())
    lastAssignedY += FormatTranslatorsConstants::Y_INCREMENT;
}
}  // namespace formatTranslators
