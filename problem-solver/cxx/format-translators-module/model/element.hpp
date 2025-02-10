#pragma once

#include "identifier_position.hpp"

#include <list>
#include <string>
#include <memory>

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_type.hpp>

namespace formatTranslators
{
class Connector;
class Node;
class Visitor;

class Element
{
public:
  Element();

  std::string const & getIdentifier() const;

  void setIdentifier(std::string const & identifier);

  IdentifierPosition getIdentifierPosition() const;

  void setIdentifierPosition(IdentifierPosition identifierPosition);

  void addConnector(std::shared_ptr<Connector> const & connector);

  std::list<std::shared_ptr<Connector>> const & getConnectors() const;

  ScAddr const & getScAddress() const;

  void setScAddress(ScAddr const & scAddress);

  ScType const & getScType() const;

  void setScType(ScType const & scType);

  virtual void acceptVisitor(Visitor & visitor) = 0;

  virtual std::string getIdForBaseElement() const;

  virtual std::string getIdForOtherElement() const;

  std::string getId() const;

  void setId(std::string const & id);

private:
  std::string id;
  ScAddr scAddress;
  ScType scType;
  std::list<std::shared_ptr<Connector>> connectors;
  std::string identifier;
  IdentifierPosition identifierPosition;
};

}  // namespace formatTranslators
