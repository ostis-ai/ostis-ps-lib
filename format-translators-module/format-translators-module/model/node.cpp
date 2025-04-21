#include "node.hpp"
#include "connector.hpp"

#include "visitor/visitor.hpp"

namespace formatTranslators
{
Node::Node()
  : x(0)
  , topY(0)
  , bottomY(0)
{
}

void Node::AcceptVisitor(Visitor & visitor)
{
  visitor.VisitNode(*this);
}

std::string const & Node::GetContent() const
{
  static std::string EMPTY_CONTENT;
  return EMPTY_CONTENT;
}

float Node::GetX() const
{
  return x;
}

void Node::SetX(float x)
{
  this->x = x;
}

float Node::GetTopY() const
{
  return topY;
}

void Node::SetTopY(float topY)
{
  this->topY = topY;
}

float Node::GetBottomY() const
{
  return bottomY;
}

void Node::SetBottomY(float bottomY)
{
  this->bottomY = bottomY;
}

bool Node::HasBus() const
{
  return GetConnectors().size() > 1;
}

std::string Node::GetIdForBaseElement() const
{
  return HasBus() ? GetBusId() : GetNodeId();
}

std::string Node::GetIdForOtherElement() const
{
  return GetNodeId();
}

std::string Node::GetNodeId() const
{
  return GetId();
}

std::string Node::GetBusId() const
{
  return GetId() + "_bus";
}

std::shared_ptr<Node> Node::Copy() const
{
  std::shared_ptr<Node> node = std::make_shared<Node>();
  Copy(*node);
  return node;
}

void Node::Copy(Node & copiedNode) const
{
  copiedNode.SetScAddress(GetScAddress());
  copiedNode.SetScType(GetScType());
  copiedNode.SetIdentifier(GetIdentifier());
  copiedNode.SetIdentifierPosition(GetIdentifierPosition());
  copiedNode.SetBottomY(GetBottomY());
  copiedNode.SetTopY(GetTopY());
}

}  // namespace formatTranslators
