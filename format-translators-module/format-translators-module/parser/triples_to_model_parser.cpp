#include <sc-memory/sc_keynodes.hpp>

#include "model/connector.hpp"
#include "model/node.hpp"
#include "model/link.hpp"

#include "triples_to_model_parser.hpp"

namespace formatTranslators
{

std::shared_ptr<Node> TriplesToModelParser::ParseTriples(
    ScMemoryContext * memoryContext,
    ScAddr const & root,
    ScAddrToValueUnorderedMap<uint32_t> const & keyElementsOrderMap,
    ScAddr const & identifiersLanguageAddr,
    std::shared_ptr<Triples> const & structureTriplesMap)
{
  context = memoryContext;
  keyElementsOrder = keyElementsOrderMap;
  structureTriples = structureTriplesMap;
  identifiersLanguage = identifiersLanguageAddr;
  return WalkBFS(root);
}

std::shared_ptr<Node> TriplesToModelParser::WalkBFS(ScAddr const & root)
{
  ScType const & rootType = context->GetElementType(root);
  // TODO(kilativ-dotcom): allow connectors to be roots
  if (!rootType.IsNode())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Only node is allowed to be a root, got " << std::string(rootType) << " instead for root with hash `"
                                                  << root.Hash() << "`.");
  auto rootElement = CreateNode(root, rootType);
  if (rootElements.count(root))
    return rootElement;
  rootElements.insert(root);
  auto const & pairsIterator = structureTriples->find(root);
  if (pairsIterator == structureTriples->cend())
    return rootElement;
  for (auto const & [_, otherElementAndConnectorWithReverse] : pairsIterator->second)
  {
    for (auto const & [otherElementAddr, connectorAddr, isReversed] : otherElementAndConnectorWithReverse)
    {
      if (walkedConnectors.count(connectorAddr))
        continue;
      walkedConnectors.insert(connectorAddr);
      // TODO(): if otherElement is Structure with nodes + connectors then it probably can be displayed as a structure
      auto const & subtreeRoot = WalkBFS(otherElementAddr);
      auto const & connectorElement = CreateConnector(connectorAddr, rootElement, subtreeRoot, isReversed);
      ProcessTriple(rootElement, connectorElement);
    }
  }
  return rootElement;
}

void TriplesToModelParser::ProcessTriple(
    std::shared_ptr<Node> const & rootElement,
    std::shared_ptr<Connector> const & connectorElement)
{
  rootElement->AddConnector(connectorElement);

  auto const & pairsIterator = structureTriples->find(connectorElement->GetScAddress());
  if (pairsIterator != structureTriples->cend())
  {
    for (auto const & [_, otherElementAndConnectorWithReverse] : pairsIterator->second)
      for (auto const & [relationAddr, connectorAddr, isReversed] : otherElementAndConnectorWithReverse)
        AddVerticalConnector(relationAddr, connectorAddr, connectorElement, isReversed);
  }
}

std::shared_ptr<Node> TriplesToModelParser::CreateNode(ScAddr const & nodeAddr, ScType const & nodeType) const
{
  if (!nodeType.IsNode())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidParams,
        "Cannot create node with type " << std::string(nodeType) << " for sc-element with hash `" << nodeAddr.Hash()
                                        << "`.");
  std::shared_ptr<Node> nodeElement;
  if (nodeType.IsLink())
  {
    auto const linkElement = std::make_shared<Link>();
    std::string linkContent;
    context->GetLinkContent(nodeAddr, linkContent);
    linkElement->SetContent(linkContent);
    nodeElement = linkElement;
  }
  else
    nodeElement = std::make_shared<Node>();
  nodeElement->SetScAddress(nodeAddr);
  nodeElement->SetScType(context->GetElementType(nodeAddr));
  SetIdentifierIfExists(nodeAddr, nodeElement);

  return nodeElement;
}

void TriplesToModelParser::SetIdentifierIfExists(ScAddr const & elementAddr, std::shared_ptr<Element> const & element)
    const
{
  std::string identifier;
  auto const & mainIdentifierIterator = context->CreateIterator5(
      elementAddr, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, ScKeynodes::nrel_main_idtf);
  while (mainIdentifierIterator->Next() && identifier.empty())
  {
    ScAddr const & link = mainIdentifierIterator->Get(2);
    if (context->CheckConnector(identifiersLanguage, link, ScType::ConstPermPosArc))
      context->GetLinkContent(link, identifier);
  }
  if (identifier.empty())
    identifier = context->GetElementSystemIdentifier(elementAddr);
  if (!identifier.empty())
    element->SetIdentifier(identifier);
  element->SetIdentifierPosition(IdentifierPosition::BOTTOM_RIGHT);
}

std::shared_ptr<Connector> TriplesToModelParser::CreateConnector(
    ScAddr const & connector,
    std::shared_ptr<Element> const & baseElement,
    std::shared_ptr<Node> const & otherElement,
    bool const isReversed) const
{
  auto const & connectorElement = std::make_shared<Connector>();
  connectorElement->SetScAddress(connector);
  connectorElement->SetScType(context->GetElementType(connector));

  SetIdentifierIfExists(connector, connectorElement);

  connectorElement->SetBaseElement(baseElement);
  connectorElement->SetOtherElement(otherElement);
  connectorElement->SetIsReversed(isReversed);

  return connectorElement;
}

void TriplesToModelParser::AddVerticalConnector(
    ScAddr const & otherAddr,
    ScAddr const & connectorAddr,
    std::shared_ptr<Connector> const & baseConnector,
    bool const isReversed) const
{
  auto const otherElement = CreateNode(otherAddr, context->GetElementType(otherAddr));
  auto const & connectorElement = CreateConnector(connectorAddr, baseConnector, otherElement, isReversed);
  baseConnector->AddConnector(connectorElement);
}

}  // namespace formatTranslators
