#include "node.hpp"
#include "connector.hpp"

#include "visitor/visitor.hpp"

namespace formatTranslators
{
void Node::acceptVisitor(Visitor & visitor)
{
  visitor.visitNode(*this);
}

std::string const & Node::getContent() const
{
  static std::string EMPTY_CONTENT;
  return EMPTY_CONTENT;
}

Node::Node()
  : Element()
{
}

float Node::getX() const
{
  return x;
}

void Node::setX(float x)
{
  this->x = x;
}

float Node::getTopY() const
{
  return topY;
}

void Node::setTopY(float topY)
{
  this->topY = topY;
}

float Node::getBottomY() const
{
  return bottomY;
}

void Node::setBottomY(float bottomY)
{
  this->bottomY = bottomY;
}

bool Node::hasBus() const
{
  return getConnectors().size() > 1;
}

std::string Node::getIdForBaseElement() const
{
  return hasBus() ? getBusId() : getNodeId();
}

std::string Node::getIdForOtherElement() const
{
  return getNodeId();
}

std::string Node::getNodeId() const
{
  return getId();
}

std::string Node::getBusId() const
{
  return getId() + "_bus";
}

size_t Node::calculateReservedVerticalElementsOnParent()
{
  if (reservedVerticalElementsOnParent == 0)
  {
    for (auto const & connector : getConnectors())
      reservedVerticalElementsOnParent += connector->getOtherElement()->calculateReservedVerticalElementsOnParent();
    if (hasBus())
      reservedVerticalElementsOnParent++;
    if (reservedVerticalElementsOnParent == 0)
      reservedVerticalElementsOnParent = 1;
  }
  return reservedVerticalElementsOnParent;
}

std::shared_ptr<Node> Node::copy() const
{
  std::shared_ptr<Node> node = std::make_shared<Node>();
  copy(*node);
  return node;
}

void Node::copy(Node & copiedNode) const
{
  copiedNode.setScAddress(getScAddress());
  copiedNode.setScType(getScType());
  copiedNode.setIdentifier(getIdentifier());
  copiedNode.setIdentifierPosition(getIdentifierPosition());
  copiedNode.setBottomY(getBottomY());
  copiedNode.setTopY(getTopY());
}

}  // namespace formatTranslators
