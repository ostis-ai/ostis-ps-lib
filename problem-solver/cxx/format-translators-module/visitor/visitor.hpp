#pragma once

namespace formatTranslators
{
class Connector;
class Link;
class Node;

class Visitor
{
public:
  virtual void VisitConnector(Connector & connector) = 0;
  virtual void VisitLink(Link & link) = 0;
  virtual void VisitNode(Node & node) = 0;

private:
};

}  // namespace formatTranslators
