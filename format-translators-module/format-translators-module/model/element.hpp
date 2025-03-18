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

  virtual ~Element() = default;

  virtual void AcceptVisitor(Visitor & visitor) = 0;

  std::string const & GetIdentifier() const;

  void SetIdentifier(std::string const & identifier);

  IdentifierPosition GetIdentifierPosition() const;

  void SetIdentifierPosition(IdentifierPosition identifierPosition);

  void AddConnector(std::shared_ptr<Connector> const & connector);

  std::list<std::shared_ptr<Connector>> const & GetConnectors() const;

  ScAddr const & GetScAddress() const;

  void SetScAddress(ScAddr const & scAddress);

  ScType const & GetScType() const;

  void SetScType(ScType const & scType);

  virtual std::string GetIdForBaseElement() const;

  virtual std::string GetIdForOtherElement() const;

  std::string GetId() const;

  void SetId(std::string const & id);

private:
  std::string id;
  ScAddr scAddress;
  ScType scType;
  std::list<std::shared_ptr<Connector>> connectors;
  std::string identifier;
  IdentifierPosition identifierPosition;
};

}  // namespace formatTranslators
