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
float const SCgVisualisationTranslatorAgent::Y_INCREMENT = 100;

ScAddr SCgVisualisationTranslatorAgent::GetActionClass() const noexcept(false)
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
    parseKeyElementsOrder(keyElementsOrderTuple);
    walkBFS(mainKeyElement, 0);
    assignXCoordinates();
    GWFTranslatorVisitor visitor;
    parsedStructure[0].front()->acceptVisitor(visitor);
    ScAddr const & resultLink = m_context.GenerateLink(ScType::ConstNodeLink);
    m_context.SetLinkContent(
        resultLink,
        GWF_CONTENT_BEFORE_STATIC_SECTOR_CONTENT + visitor.getStaticSectorContent()
            + GWF_CONTENT_AFTER_STATIC_SECTOR_CONTENT);
    action.FormResult(resultLink);
    return action.FinishSuccessfully();
  }
  catch (utils::ScException const & exception)
  {
    SC_AGENT_LOG_ERROR(exception.Description());
    return action.FinishWithError();
  }
}

void SCgVisualisationTranslatorAgent::parseKeyElementsOrder(ScAddr const & keyElementsOrderTuple)
{
  ScAddrUnorderedSet visited;
  auto const & firstElementIterator = m_context.CreateIterator5(
      keyElementsOrderTuple, ScType::ConstPermPosArc, ScType::Node, ScType::ConstPermPosArc, ScKeynodes::rrel_1);
  if (firstElementIterator->Next())
  {
    ScAddr element = firstElementIterator->Get(2);
    ScAddr arcToElement = firstElementIterator->Get(1);
    while (m_context.IsElement(arcToElement))
    {
      if (!m_context.GetElementType(arcToElement).IsArc())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Ordered set is not formed correct, arc has type " << std::string(m_context.GetElementType(arcToElement)));
      element = m_context.GetArcTargetElement(arcToElement);
      if (visited.count(arcToElement))
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Ordered set contains loop");
      if (!m_context.GetElementType(element).IsNode())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Key elements order has element with type " << std::string(m_context.GetElementType(arcToElement))
                                                        << " instead of node");
      visited.insert(arcToElement);
      keyElementsOrder.push_back(element);
      arcToElement =
          utils::IteratorUtils::getAnyByOutRelation(&m_context, arcToElement, ScKeynodes::nrel_basic_sequence);
    }
  }
}

std::shared_ptr<Node> SCgVisualisationTranslatorAgent::walkBFS(ScAddr const & root, size_t currentLevel)
{
  ScType const & rootType = m_context.GetElementType(root);
  if (!rootType.IsNode())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Only node is allowed to be a root, got " << std::string(rootType) << " instead");
  auto rootElement = createNode(root, rootType);
  rootElement->setTopY(lastAssignedY);
  if (parsedStructure.size() <= currentLevel)
    parsedStructure.resize(currentLevel + 1);
  parsedStructure[currentLevel].push_back(rootElement);
  if (rootElements.count(root))
  {
    rootElement->setBottomY(lastAssignedY);
    return rootElement;
  }
  rootElements.insert(root);
  for (auto const & keyElement : keyElementsOrder)
  {
    auto const & incomingTriplesIterator =
        m_context.CreateIterator5(keyElement, ScType::Connector, root, ScType::ConstPermPosArc, structureToTranslate);
    processIncomingIterator(rootElement, incomingTriplesIterator, currentLevel, {});
    auto const & outgoingTriplesIterator =
        m_context.CreateIterator5(root, ScType::Connector, keyElement, ScType::ConstPermPosArc, structureToTranslate);
    processOutgoingIterator(rootElement, outgoingTriplesIterator, currentLevel, {});
    auto const & incomingQuintuplesIterator =
        m_context.CreateIterator5(ScType::Node, ScType::Connector, root, ScType::Connector, keyElement);
    processIncomingIterator(rootElement, incomingQuintuplesIterator, currentLevel, {3});
    auto const & outgoingQuintuplesIterator =
        m_context.CreateIterator5(root, ScType::Connector, ScType::Node, ScType::Connector, keyElement);
    processOutgoingIterator(rootElement, outgoingQuintuplesIterator, currentLevel, {3});
  }
  if (rootElement->hasBus())
  {
    rootElement->setBottomY(increaseSubtreeYCoordinate(rootElement, Y_INCREMENT));
    lastAssignedY += Y_INCREMENT;
  }
  else
    rootElement->setBottomY(rootElement->getTopY());
  return rootElement;
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
void SCgVisualisationTranslatorAgent::processIncomingIterator(
    std::shared_ptr<Node> & rootElement,
    std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> const & incomingIterator,
    size_t const currentLevel,
    std::list<size_t> const & indicesForStructureCheck)
{
  processIterator(rootElement, incomingIterator, currentLevel, indicesForStructureCheck, true);
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
void SCgVisualisationTranslatorAgent::processOutgoingIterator(
    std::shared_ptr<Node> & rootElement,
    std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> const & outgoingIterator,
    size_t const currentLevel,
    std::list<size_t> const & indicesForStructureCheck)
{
  processIterator(rootElement, outgoingIterator, currentLevel, indicesForStructureCheck, false);
}

template <typename ParamType1, typename ParamType2, typename ParamType3, typename ParamType4, typename ParamType5>
void SCgVisualisationTranslatorAgent::processIterator(
    std::shared_ptr<Node> & rootElement,
    std::shared_ptr<ScIterator5<ParamType1, ParamType2, ParamType3, ParamType4, ParamType5>> const & iterator,
    size_t const currentLevel,
    std::list<size_t> const & indicesForStructureCheck,
    bool const isIncoming)
{
  while (iterator->Next())
  {
    if (std::any_of(
            indicesForStructureCheck.cbegin(),
            indicesForStructureCheck.cend(),
            [this, &iterator](size_t const index)
            {
              return !m_context.CheckConnector(structureToTranslate, iterator->Get(index), ScType::ConstPermPosArc);
            }))
      continue;
    ScAddr const & connectorAddr = iterator->Get(1);
    if (walkedConnectors.count(connectorAddr))
      continue;
    walkedConnectors.insert(connectorAddr);
    if (!m_context.CheckConnector(structureToTranslate, connectorAddr, ScType::ConstPermPosArc))
      continue;
    auto const & subtreeRoot = walkBFS(iterator->Get(isIncoming ? 0 : 2), currentLevel + 1);
    auto const & connectorElement = createConnector(connectorAddr, rootElement, subtreeRoot, isIncoming);
    processTriple(rootElement, connectorElement, subtreeRoot);
  }
}

void SCgVisualisationTranslatorAgent::processTriple(
    std::shared_ptr<Node> & rootElement,
    std::shared_ptr<Connector> const & connectorElement,
    std::shared_ptr<Node> const & subtreeRoot)
{
  connectorElement->setBaseElementY(subtreeRoot->getTopY());
  connectorElement->setOtherElementY(subtreeRoot->getTopY());
  rootElement->addConnector(connectorElement);
  addIncomingVerticalConnectors(connectorElement);
  addOutgoingVerticalConnectors(connectorElement);
  if (subtreeRoot->getConnectors().empty())
    lastAssignedY += Y_INCREMENT;
}

std::shared_ptr<Node> SCgVisualisationTranslatorAgent::createNode(ScAddr const & nodeAddr, ScType const & nodeType)
    const
{
  if (!nodeType.IsNode())
    SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Cannot create node with type " << std::string(nodeType));
  std::shared_ptr<Node> nodeElement;
  if (nodeType.IsLink())
  {
    auto linkElement = std::make_shared<Link>();
    std::string linkContent;
    m_context.GetLinkContent(nodeAddr, linkContent);
    linkElement->setContent(linkContent);
    nodeElement = linkElement;
  }
  else
    nodeElement = std::make_shared<Node>();
  nodeElement->setScAddress(nodeAddr);
  nodeElement->setScType(m_context.GetElementType(nodeAddr));
  setSystemIdentifierIfExists(nodeAddr, nodeElement);

  return nodeElement;
}

void SCgVisualisationTranslatorAgent::setSystemIdentifierIfExists(
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
    element->setIdentifier(identifier);
  element->setIdentifierPosition(IdentifierPosition::BOTTOM_RIGHT);
}

std::shared_ptr<Connector> SCgVisualisationTranslatorAgent::createConnector(
    ScAddr const & connector,
    std::shared_ptr<Element> const & baseElement,
    std::shared_ptr<Node> const & otherElement,
    bool const isReversed) const
{
  auto const & connectorElement = std::make_shared<Connector>();
  connectorElement->setScAddress(connector);
  connectorElement->setScType(m_context.GetElementType(connector));

  setSystemIdentifierIfExists(connector, connectorElement);

  connectorElement->setBaseElement(baseElement);
  connectorElement->setOtherElement(otherElement);
  connectorElement->setIsReversed(isReversed);

  return connectorElement;
}

void SCgVisualisationTranslatorAgent::addIncomingVerticalConnectors(
    std::shared_ptr<Connector> const & connectorElement) const
{
  for (auto const & keyElement : keyElementsOrder)
  {
    auto const & incomingIterator = m_context.CreateIterator5(
        keyElement, ScType::Connector, connectorElement->getScAddress(), ScType::ConstPermPosArc, structureToTranslate);
    while (incomingIterator->Next())
      addVerticalConnector(keyElement, incomingIterator->Get(1), connectorElement, true);
  }
}

void SCgVisualisationTranslatorAgent::addOutgoingVerticalConnectors(
    std::shared_ptr<Connector> const & connectorElement) const
{
  for (auto const & keyElement : keyElementsOrder)
  {
    auto const & outgoingIterator = m_context.CreateIterator5(
        connectorElement->getScAddress(), ScType::Connector, keyElement, ScType::ConstPermPosArc, structureToTranslate);
    while (outgoingIterator->Next())
      addVerticalConnector(keyElement, outgoingIterator->Get(1), connectorElement, false);
  }
}

void SCgVisualisationTranslatorAgent::addVerticalConnector(
    ScAddr const & otherAddr,
    ScAddr const & connectorAddr,
    std::shared_ptr<Connector> const & baseConnector,
    bool const isReversed) const
{
  auto otherElement = createNode(otherAddr, m_context.GetElementType(otherAddr));
  otherElement->setTopY(baseConnector->getOtherElementY() - Y_INCREMENT / 2);
  otherElement->setBottomY(otherElement->getTopY());
  auto const & connectorElement = createConnector(connectorAddr, baseConnector, otherElement, isReversed);
  connectorElement->setOtherElementY(otherElement->getTopY());
  connectorElement->setBaseElementY(baseConnector->getOtherElementY());
  baseConnector->addConnector(connectorElement);
}

float SCgVisualisationTranslatorAgent::increaseSubtreeYCoordinate(
    std::shared_ptr<Node> const & treeRoot,
    float const increment) const
{
  float maxY = 0;
  for (auto const & connector : treeRoot->getConnectors())
  {
    for (auto const & connectorConnector : connector->getConnectors())
    {
      auto const & relationElement = connectorConnector->getOtherElement();
      relationElement->setTopY(relationElement->getTopY() + increment);
      relationElement->setBottomY(relationElement->getTopY());
      connectorConnector->setOtherElementY(relationElement->getTopY());
    }
    auto const & otherElement = connector->getOtherElement();
    otherElement->setTopY(otherElement->getTopY() + increment);
    otherElement->setBottomY(otherElement->getBottomY() + increment);
    maxY = std::max(otherElement->getBottomY(), maxY);
    connector->setOtherElementY(otherElement->getTopY());
    maxY = std::max(increaseSubtreeYCoordinate(otherElement, increment), maxY);
  }
  return maxY;
}

void SCgVisualisationTranslatorAgent::assignXCoordinates() const
{
  float previousLevelIndent = 0;
  float nextLevelIndent = 0;
  float relationIndent = 0;
  float scaleX = 5;
  float connectorPercent = 0.7f;
  // levelElementConnectorIdentifierScale was set after a couple of experiments because it makes connector long enough
  // for identifier to not overlap with other element
  float levelElementConnectorIdentifierScale = 0.4f;
  if (!parsedStructure.empty() && !parsedStructure[0].empty())
    parsedStructure[0].front()->setX(0);
  for (size_t level = 0; level + 1 < parsedStructure.size(); ++level)
  {
    float maxRelationIdentifierLength = 0;
    float maxLevelElementIdentifierLength = 0;
    for (auto const & levelElement : parsedStructure[level])
    {
      if (levelElement->getIdentifier().size() > maxLevelElementIdentifierLength)
        maxLevelElementIdentifierLength = levelElement->getIdentifier().size();

      for (auto const & levelElementConnector : levelElement->getConnectors())
      {
        if (levelElementConnector->getIdentifier().size() > maxRelationIdentifierLength)
          maxRelationIdentifierLength =
              levelElementConnector->getIdentifier().size() / levelElementConnectorIdentifierScale;
        for (auto const & levelElementConnectorConnector : levelElementConnector->getConnectors())
        {
          if (levelElementConnectorConnector->getIdentifier().size() > maxRelationIdentifierLength)
            maxRelationIdentifierLength = levelElementConnectorConnector->getIdentifier().size() / connectorPercent;
          if (levelElementConnectorConnector->getOtherElement()->getIdentifier().size() > maxRelationIdentifierLength)
            maxRelationIdentifierLength =
                levelElementConnectorConnector->getOtherElement()->getIdentifier().size() / connectorPercent;
        }
      }
    }
    previousLevelIndent = nextLevelIndent;
    nextLevelIndent += (std::max(maxLevelElementIdentifierLength, maxRelationIdentifierLength) + 8) * scaleX;
    relationIndent = nextLevelIndent - (nextLevelIndent - previousLevelIndent) * (connectorPercent);
    for (auto const & nextLevelElement : parsedStructure[level + 1])
      nextLevelElement->setX(nextLevelIndent);
    for (auto const & levelElement : parsedStructure[level])
    {
      float levelElementConnectorIndex = 1;
      size_t levelElementConnectorsSize = levelElement->calculateReservedVerticalElementsOnParent() - 1;
      for (auto const & levelElementConnector : levelElement->getConnectors())
      {
        levelElementConnector->setBaseElementBalance(levelElementConnectorIndex / levelElementConnectorsSize - 0.001f);
        for (auto const & levelElementConnectorConnector : levelElementConnector->getConnectors())
        {
          levelElementConnectorConnector->setOtherElementX(relationIndent);
          if (levelElementConnector->getIsReversed())
            levelElementConnectorConnector->setBaseElementBalance(connectorPercent);
          else
            levelElementConnectorConnector->setBaseElementBalance(1 - connectorPercent);
        }
        levelElementConnectorIndex +=
            levelElementConnector->getOtherElement()->calculateReservedVerticalElementsOnParent();
      }
    }
  }
}

}  // namespace formatTranslators
