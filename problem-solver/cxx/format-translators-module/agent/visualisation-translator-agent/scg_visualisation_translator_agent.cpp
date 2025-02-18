#include "scg_visualisation_translator_agent.hpp"

#include "keynodes/format_translators_keynodes.hpp"

#include <sc-agents-common/utils/IteratorUtils.hpp>

#include "model/node.hpp"
#include "model/link.hpp"
#include "model/connector.hpp"

#include "visitor/gwf_translator_visitor.hpp"

namespace formatTranslators
{
std::string const SCgVisualisationTranslatorAgent::GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT =
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<GWF version=\"2.0\">\n"
    "    <staticSector>\n";
std::string const SCgVisualisationTranslatorAgent::GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT =
    "    </staticSector>\n"
    "</GWF>\n";
float const SCgVisualisationTranslatorAgent::CONNECTOR_IDENTIFIER_SCALE = 0.4f;
float const SCgVisualisationTranslatorAgent::CONNECTOR_INCIDENT_POINT_PERCENT = 0.7f;
float const SCgVisualisationTranslatorAgent::Y_INCREMENT = 100;
float const SCgVisualisationTranslatorAgent::MIN_X = 200;
float const SCgVisualisationTranslatorAgent::MAX_Y = 800;
float const SCgVisualisationTranslatorAgent::MAX_LINK_WIDTH = 60;

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
    identifiersLanguage = action.GetArgument(2, FormatTranslatorsKeynodes::lang_en);
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
    ParseStructureIntoTriples();
    auto const & rootElement = WalkBFS(mainKeyElement, 0);
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
        GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT + visitor.GetStaticSectorContent()
            + GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT);
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
            "Ordered set is not formed correct, membership arc has type "
                << std::string(m_context.GetElementType(arcToElement)));
      ScAddr const & element = m_context.GetArcTargetElement(arcToElement);
      if (visited.count(arcToElement))
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Ordered set contains loop");
      if (!m_context.GetElementType(element).IsNode())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Key elements order has element with type " << std::string(m_context.GetElementType(arcToElement))
                                                        << " instead of node");
      visited.insert(arcToElement);
      keyElementsOrder[element] = keyElementsOrder.size();
      arcToElement =
          utils::IteratorUtils::getAnyByOutRelation(&m_context, arcToElement, ScKeynodes::nrel_basic_sequence);
    }
  }
}

void SCgVisualisationTranslatorAgent::ParseStructureIntoTriples()
{
  auto const & structureConnectorsIterator =
      m_context.CreateIterator3(structureToTranslate, ScType::ConstPermPosArc, ScType::Connector);
  std::list<std::tuple<ScAddr, ScAddr, ScAddr>> triplesWithConnectors;
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> triplesWithConnectorNotInParsedStructure;
  ScAddrToValueUnorderedMap<std::pair<ScAddr, ScAddr>> triplesWithConnectorInParsedStructure;
  while (structureConnectorsIterator->Next())
  {
    ScAddr const & connector = structureConnectorsIterator->Get(2);
    auto const & [first, second] = m_context.GetConnectorIncidentElements(connector);
    if (m_context.GetElementType(second).IsNode())
    {
      if (keyElementsOrder.count(second))
      {
        AddTripleToParsedTriples(first, connector, second, false);
        auto const & notInParsedIterator = triplesWithConnectorNotInParsedStructure.find(connector);
        if (notInParsedIterator != triplesWithConnectorNotInParsedStructure.cend())
        {
          triplesWithConnectorInParsedStructure.insert({connector, {first, second}});
          triplesWithConnectorNotInParsedStructure.erase(connector);
        }
      }
      else
      {
        if (structureTriples.count(connector))
          triplesWithConnectorInParsedStructure.insert({connector, {first, second}});
        else
          triplesWithConnectorNotInParsedStructure.insert({connector, {first, second}});
      }
    }

    if (m_context.GetElementType(first).IsNode())
    {
      if (keyElementsOrder.count(first))
      {
        AddTripleToParsedTriples(second, connector, first, true);
        auto const & notInParsedIterator = triplesWithConnectorNotInParsedStructure.find(connector);
        if (notInParsedIterator != triplesWithConnectorNotInParsedStructure.cend())
        {
          triplesWithConnectorInParsedStructure.insert({connector, {first, second}});
          triplesWithConnectorNotInParsedStructure.erase(connector);
        }
      }
      else
      {
        if (structureTriples.count(connector))
          triplesWithConnectorInParsedStructure.insert({connector, {first, second}});
        else
          triplesWithConnectorNotInParsedStructure.insert({connector, {first, second}});
      }
    }
  }
  for (auto const & [connector, firstAndSecond] : triplesWithConnectorInParsedStructure)
  {
    auto const & connectorIsBaseIterator = structureTriples.find(connector);
    if (connectorIsBaseIterator != structureTriples.cend())
    {
      auto const & [first, second] = firstAndSecond;
      if (m_context.GetElementType(second).IsNode())
        AddTripleToParsedTriples(first, connector, second, false);
      if (m_context.GetElementType(first).IsNode())
        AddTripleToParsedTriples(second, connector, first, true);
    }
  }
}

void SCgVisualisationTranslatorAgent::AddTripleToParsedTriples(
    ScAddr const & baseAddr,
    ScAddr const & connectorAddr,
    ScAddr const & otherElementAddr,
    bool const isReversed)
{
  if (m_context.GetElementType(otherElementAddr).IsNode())
  {
    if (!structureTriples.count(baseAddr))
    {
      ScAddrComparator const comparator(keyElementsOrder, structureTriples);
      std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>
          pairsWithComparator(comparator);
      structureTriples.insert({baseAddr, pairsWithComparator});
    }
    structureTriples.at(baseAddr)[{otherElementAddr, connectorAddr}].push_back(
        {otherElementAddr, connectorAddr, isReversed});
  }
}

std::shared_ptr<Node> SCgVisualisationTranslatorAgent::WalkBFS(ScAddr const & root, size_t currentLevel)
{
  ScType const & rootType = m_context.GetElementType(root);
  if (!rootType.IsNode())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Only node is allowed to be a root, got " << std::string(rootType) << " instead");
  auto rootElement = CreateNode(root, rootType);
  if (rootElements.count(root))
    return rootElement;
  rootElements.insert(root);
  auto const & pairsIterator = structureTriples.find(root);
  if (pairsIterator == structureTriples.cend())
    return rootElement;
  for (auto const & [_, otherElementAndConnectorWithReverse] : pairsIterator->second)
  {
    for (auto const & [otherElementAddr, connectorAddr, isReversed] : otherElementAndConnectorWithReverse)
    {
      if (walkedConnectors.count(connectorAddr))
        continue;
      walkedConnectors.insert(connectorAddr);
      auto const & subtreeRoot = WalkBFS(otherElementAddr, currentLevel + 1);
      auto const & connectorElement = CreateConnector(connectorAddr, rootElement, subtreeRoot, isReversed);
      ProcessTriple(rootElement, connectorElement);
    }
  }
  return rootElement;
}

void SCgVisualisationTranslatorAgent::ProcessTriple(
    std::shared_ptr<Node> const & rootElement,
    std::shared_ptr<Connector> const & connectorElement)
{
  rootElement->AddConnector(connectorElement);

  auto const & pairsIterator = structureTriples.find(connectorElement->GetScAddress());
  if (pairsIterator != structureTriples.cend())
  {
    for (auto const & [_, otherElementAndConnectorWithReverse] : pairsIterator->second)
      for (auto const & [relationAddr, connectorAddr, isReversed] : otherElementAndConnectorWithReverse)
        AddVerticalConnector(relationAddr, connectorAddr, connectorElement, isReversed);
  }
}

std::shared_ptr<Node> SCgVisualisationTranslatorAgent::CreateNode(ScAddr const & nodeAddr, ScType const & nodeType)
    const
{
  if (!nodeType.IsNode())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Cannot create node with type " << std::string(nodeType));
  std::shared_ptr<Node> nodeElement;
  if (nodeType.IsLink())
  {
    auto const linkElement = std::make_shared<Link>();
    std::string linkContent;
    m_context.GetLinkContent(nodeAddr, linkContent);
    linkElement->SetContent(linkContent);
    nodeElement = linkElement;
  }
  else
    nodeElement = std::make_shared<Node>();
  nodeElement->SetScAddress(nodeAddr);
  nodeElement->SetScType(m_context.GetElementType(nodeAddr));
  SetSystemIdentifierIfExists(nodeAddr, nodeElement);

  return nodeElement;
}

void SCgVisualisationTranslatorAgent::SetSystemIdentifierIfExists(
    ScAddr const & elementAddr,
    std::shared_ptr<Element> const & element) const
{
  std::string identifier;
  auto const & mainIdentifierIterator = m_context.CreateIterator5(
      elementAddr, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf);
  while (mainIdentifierIterator->Next() && identifier.empty())
  {
    ScAddr const & link = mainIdentifierIterator->Get(2);
    if (m_context.CheckConnector(identifiersLanguage, link, ScType::ConstPermPosArc))
      m_context.GetLinkContent(link, identifier);
  }
  if (identifier.empty())
    identifier = m_context.GetElementSystemIdentifier(elementAddr);
  if (!identifier.empty())
    element->SetIdentifier(identifier);
  element->SetIdentifierPosition(IdentifierPosition::BOTTOM_RIGHT);
}

std::shared_ptr<Connector> SCgVisualisationTranslatorAgent::CreateConnector(
    ScAddr const & connector,
    std::shared_ptr<Element> const & baseElement,
    std::shared_ptr<Node> const & otherElement,
    bool const isReversed) const
{
  auto const & connectorElement = std::make_shared<Connector>();
  connectorElement->SetScAddress(connector);
  connectorElement->SetScType(m_context.GetElementType(connector));

  SetSystemIdentifierIfExists(connector, connectorElement);

  connectorElement->SetBaseElement(baseElement);
  connectorElement->SetOtherElement(otherElement);
  connectorElement->SetIsReversed(isReversed);

  return connectorElement;
}

void SCgVisualisationTranslatorAgent::AddVerticalConnector(
    ScAddr const & otherAddr,
    ScAddr const & connectorAddr,
    std::shared_ptr<Connector> const & baseConnector,
    bool const isReversed) const
{
  auto const otherElement = CreateNode(otherAddr, m_context.GetElementType(otherAddr));
  auto const & connectorElement = CreateConnector(connectorAddr, baseConnector, otherElement, isReversed);
  baseConnector->AddConnector(connectorElement);
}

std::list<std::shared_ptr<Node>> SCgVisualisationTranslatorAgent::AssignXCoordinates(
    std::shared_ptr<Node> const & rootElement)
{
  float previousLevelMaxLinkContentSize = rootElement->GetContent().size();
  if (previousLevelMaxLinkContentSize > MAX_LINK_WIDTH)
    previousLevelMaxLinkContentSize = MAX_LINK_WIDTH;
  float nextLevelIndent = MIN_X;
  float const scaleX = 6;
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
          (std::max(maxLevelElementIdentifierLength, maxRelationIdentifierLength) + indentCausedByLinkContent + 8)
          * scaleX;
      nextLevelIndent += indentDifference;
      bool const reachedEndOfIndent = nextLevelIndent >= MAX_Y;
      if (reachedEndOfIndent)
        UpdateNextLevelRoots(treeRoots, connectorsToNextLevelRoots, nextLevelRoots);

      for (auto const & nextLevelElement : nextLevelRoots)
        nextLevelElement->SetX(nextLevelIndent);
      float const relationIndent = nextLevelIndent - indentDifference * CONNECTOR_INCIDENT_POINT_PERCENT;
      SetRelationIndent(currentLevelRoots, relationIndent);
      if (reachedEndOfIndent)
      {
        nextLevelIndent = MIN_X;
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
    if (nextLevelElement->GetContent().size() > indentCausedByLinkContent)
      indentCausedByLinkContent = nextLevelElement->GetContent().size();
  }
  auto const & maxNextLevelLinkContentSize = std::min(indentCausedByLinkContent, MAX_LINK_WIDTH);
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
  for (auto const & levelElement : currentLevelRoots)
  {
    if (levelElement->GetIdentifier().size() > maxLevelElementIdentifierLength)
      maxLevelElementIdentifierLength = levelElement->GetIdentifier().size();

    for (auto const & levelElementConnector : levelElement->GetConnectors())
    {
      connectorsToNextLevelRoots.push_back(levelElementConnector);
      nextLevelRoots.push_back(levelElementConnector->GetOtherElement());
      auto const levelElementConnectorIdentifierLength =
          levelElementConnector->GetIdentifier().size() / CONNECTOR_IDENTIFIER_SCALE;
      if (levelElementConnectorIdentifierLength > maxRelationIdentifierLength)
        maxRelationIdentifierLength = levelElementConnectorIdentifierLength;
      for (auto const & levelElementConnectorConnector : levelElementConnector->GetConnectors())
      {
        auto const levelElementConnectorConnectorIdentifierLength =
            levelElementConnectorConnector->GetIdentifier().size() / CONNECTOR_INCIDENT_POINT_PERCENT;
        if (levelElementConnectorConnectorIdentifierLength > maxRelationIdentifierLength)
          maxRelationIdentifierLength = levelElementConnectorConnectorIdentifierLength;
        auto const levelElementConnectorConnectorOtherElementIdentifierLength =
            levelElementConnectorConnector->GetOtherElement()->GetIdentifier().size()
            / CONNECTOR_INCIDENT_POINT_PERCENT;
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
    float relationIndent)
{
  for (auto const & levelElement : currentLevelRoots)
  {
    for (auto const & levelElementConnector : levelElement->GetConnectors())
    {
      for (auto const & levelElementConnectorConnector : levelElementConnector->GetConnectors())
      {
        levelElementConnectorConnector->SetOtherElementX(relationIndent);
        if (levelElementConnector->GetIsReversed())
          levelElementConnectorConnector->SetBaseElementBalance(CONNECTOR_INCIDENT_POINT_PERCENT);
        else
          levelElementConnectorConnector->SetBaseElementBalance(1 - CONNECTOR_INCIDENT_POINT_PERCENT);
      }
    }
  }
}

void SCgVisualisationTranslatorAgent::AssignYCoordinates(std::shared_ptr<Node> const & rootElement)
{
  rootElement->SetTopY(lastAssignedY);
  if (rootElement->HasBus())
    lastAssignedY += Y_INCREMENT;
  float levelElementConnectorIndex = 1;
  size_t rootElementConnectorsSize = rootElement->CalculateReservedVerticalElementsOnParent() - 1;
  for (auto const & connector : rootElement->GetConnectors())
  {
    float const relationElementY = lastAssignedY - Y_INCREMENT / 2;
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

  rootElement->SetBottomY(rootElement->HasBus() ? lastAssignedY - Y_INCREMENT : lastAssignedY);
  if (rootElement->GetConnectors().empty())
    lastAssignedY += Y_INCREMENT;
}

SCgVisualisationTranslatorAgent::ScAddrComparator::ScAddrComparator(
    ScAddrToValueUnorderedMap<size_t> & order,
    ScAddrToValueUnorderedMap<
        std::map<std::pair<ScAddr, ScAddr>, std::list<std::tuple<ScAddr, ScAddr, bool>>, ScAddrComparator>> &
        structureTriples)
  : order(order)
  , structureTriples(structureTriples)
{
}

size_t SCgVisualisationTranslatorAgent::ScAddrComparator::GetValue(std::pair<ScAddr, ScAddr> const & key) const
{
  if (order.count(key.first))
    return order.at(key.first);
  ScAddr const & firstOtherElementForConnector = structureTriples.at(key.second).begin()->first.first;
  if (order.count(firstOtherElementForConnector))
    return order.at(firstOtherElementForConnector);
  return key.first.Hash() + key.second.Hash();
}

bool SCgVisualisationTranslatorAgent::ScAddrComparator::operator()(
    std::pair<ScAddr, ScAddr> const & first,
    std::pair<ScAddr, ScAddr> const & second) const
{
  size_t const firstValue = GetValue(first);
  size_t const secondValue = GetValue(second);
  return firstValue < secondValue;
}
}  // namespace formatTranslators
