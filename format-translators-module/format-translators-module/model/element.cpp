#include "element.hpp"

namespace formatTranslators
{
Element::Element() {}

std::string const & Element::GetIdentifier() const
{
  return identifier;
}

void Element::SetIdentifier(std::string const & identifier)
{
  this->identifier = identifier;
}

void Element::AddConnector(std::shared_ptr<Connector> const & connector)
{
  connectors.push_back(connector);
}

ScAddr const & Element::GetScAddress() const
{
  return scAddress;
}

void Element::SetScAddress(ScAddr const & scAddress)
{
  this->scAddress = scAddress;
}

std::list<std::shared_ptr<Connector>> const & Element::GetConnectors() const
{
  return connectors;
}

std::string Element::GetIdForBaseElement() const
{
  return GetId();
}

std::string Element::GetIdForOtherElement() const
{
  return GetId();
}

std::string Element::GetId() const
{
  return id;
}

void Element::GetId(std::string const & id)
{
  this->id = id;
}

IdentifierPosition Element::GetIdentifierPosition() const
{
  return identifierPosition;
}

void Element::SetIdentifierPosition(IdentifierPosition identifierPosition)
{
  this->identifierPosition = identifierPosition;
}

ScType const & Element::GetScType() const
{
  return scType;
}

void Element::SetScType(ScType const & scType)
{
  this->scType = scType;
}
}  // namespace formatTranslators
