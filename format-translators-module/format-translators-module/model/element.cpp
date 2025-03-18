#include "element.hpp"

namespace formatTranslators
{
Element::Element() = default;

std::string const & Element::GetIdentifier() const
{
  return identifier;
}

void Element::SetIdentifier(std::string const & identifier)
{
  this->identifier = identifier;
}

IdentifierPosition Element::GetIdentifierPosition() const
{
  return identifierPosition;
}

void Element::SetIdentifierPosition(IdentifierPosition identifierPosition)
{
  this->identifierPosition = identifierPosition;
}

void Element::AddConnector(std::shared_ptr<Connector> const & connector)
{
  connectors.push_back(connector);
}

std::list<std::shared_ptr<Connector>> const & Element::GetConnectors() const
{
  return connectors;
}

ScAddr const & Element::GetScAddress() const
{
  return scAddress;
}

void Element::SetScAddress(ScAddr const & scAddress)
{
  this->scAddress = scAddress;
}

ScType const & Element::GetScType() const
{
  return scType;
}

void Element::SetScType(ScType const & scType)
{
  this->scType = scType;
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

void Element::SetId(std::string const & id)
{
  this->id = id;
}
}  // namespace formatTranslators
