#include "element.hpp"

namespace formatTranslators
{
Element::Element() {}

std::string const & Element::getIdentifier() const
{
  return identifier;
}

void Element::setIdentifier(std::string const & identifier)
{
  this->identifier = identifier;
}

void Element::addConnector(std::shared_ptr<Connector> const & connector)
{
  connectors.push_back(connector);
}

ScAddr const & Element::getScAddress() const
{
  return scAddress;
}

void Element::setScAddress(ScAddr const & scAddress)
{
  this->scAddress = scAddress;
}

std::list<std::shared_ptr<Connector>> const & Element::getConnectors() const
{
  return connectors;
}

std::string Element::getIdForBaseElement() const
{
  return getId();
}

std::string Element::getIdForOtherElement() const
{
  return getId();
}

std::string Element::getId() const
{
  return id;
}

void Element::setId(std::string const & id)
{
  this->id = id;
}

IdentifierPosition Element::getIdentifierPosition() const
{
  return identifierPosition;
}

void Element::setIdentifierPosition(IdentifierPosition identifierPosition)
{
  this->identifierPosition = identifierPosition;
}

ScType const & Element::getScType() const
{
  return scType;
}

void Element::setScType(ScType const & scType)
{
  this->scType = scType;
}
}  // namespace formatTranslators
