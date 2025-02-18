#include "connector.hpp"

#include "visitor/visitor.hpp"

#include <sstream>
#include <sc-memory/sc_debug.hpp>

namespace formatTranslators
{
void Connector::AcceptVisitor(Visitor & visitor)
{
  visitor.VisitConnector(*this);
}

std::shared_ptr<Element> const & Connector::GetBaseElement() const
{
  return baseElement;
}

void Connector::SetBaseElement(std::shared_ptr<Element> const & baseElement)
{
  this->baseElement = baseElement;
}

std::shared_ptr<Node> const & Connector::GetOtherElement() const
{
  return otherElement;
}

void Connector::SetOtherElement(std::shared_ptr<Node> const & otherElement)
{
  this->otherElement = otherElement;
}

float Connector::GetBaseElementX() const
{
  return baseElementX;
}

void Connector::SetBaseElementX(float baseElementX)
{
  this->baseElementX = baseElementX;
}

float Connector::GetBaseElementBalance() const
{
  return baseElementBalance == 1.f ? 0.99999f : baseElementBalance;
}

void Connector::SetBaseElementBalance(float baseElementBalance)
{
  this->baseElementBalance = baseElementBalance;
}

float Connector::GetBaseElementY() const
{
  return baseElementY;
}

void Connector::SetBaseElementY(float baseElementY)
{
  this->baseElementY = baseElementY;
}

float Connector::GetOtherElementX() const
{
  return otherElementX;
}

void Connector::SetOtherElementX(float otherElementX)
{
  this->otherElementX = otherElementX;
  otherElement->SetX(otherElementX);
}

float Connector::GetOtherElementY() const
{
  return otherElementY;
}

void Connector::SetOtherElementY(float otherElementY)
{
  this->otherElementY = otherElementY;
}

float Connector::GetOtherElementBalance() const
{
  return otherElementBalance == 1.f ? 0.99999f : otherElementBalance;
}

void Connector::SetOtherElementBalance(float otherElementBalance)
{
  this->otherElementBalance = otherElementBalance;
}

bool Connector::GetIsReversed() const
{
  return isReversed;
}

void Connector::SetIsReversed(bool const isReversed)
{
  this->isReversed = isReversed;
}

std::list<std::pair<float, float>> const & Connector::GetBendPoints() const
{
  return bendPoints;
}

std::shared_ptr<Element> Connector::GetSourceElement() const
{
  return isReversed ? GetOtherElement() : GetBaseElement();
}

std::string Connector::GetSourceElementIdForConnector() const
{
  return isReversed ? GetOtherElement()->GetIdForOtherElement() : GetBaseElement()->GetIdForBaseElement();
}

float Connector::GetSourceElementX() const
{
  return isReversed ? GetOtherElementX() : GetBaseElementX();
}

float Connector::GetSourceElementY() const
{
  return isReversed ? GetOtherElementY() : GetBaseElementY();
}

float Connector::GetSourceElementBalance() const
{
  return isReversed ? GetOtherElementBalance() : GetBaseElementBalance();
}

std::shared_ptr<Element> Connector::GetTargetElement() const
{
  return isReversed ? GetBaseElement() : GetOtherElement();
}

std::string Connector::GetTargetElementIdForConnector() const
{
  return isReversed ? GetBaseElement()->GetIdForBaseElement() : GetOtherElement()->GetIdForOtherElement();
}

float Connector::GetTargetElementX() const
{
  return isReversed ? GetBaseElementX() : GetOtherElementX();
}

float Connector::GetTargetElementY() const
{
  return isReversed ? GetBaseElementY() : GetOtherElementY();
}

float Connector::GetTargetElementBalance() const
{
  return isReversed ? GetBaseElementBalance() : GetOtherElementBalance();
}
}  // namespace formatTranslators
