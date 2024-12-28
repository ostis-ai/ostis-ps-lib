#pragma once

namespace formatTranslators
{
class Connector;
class Link;
class Node;

class Visitor
{
public:
  virtual void visitConnector(Connector & connector) = 0;
  virtual void visitLink(Link & link) = 0;
  virtual void visitNode(Node & node) = 0;

private:
};

}  // namespace formatTranslators
