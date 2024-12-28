#include "connector.hpp"

#include "visitor/visitor.hpp"

#include <sstream>

namespace formatTranslators
{
void Connector::acceptVisitor(Visitor & visitor)
{
  visitor.visitConnector(*this);
}

std::shared_ptr<Element> const & Connector::getBaseElement() const
{
  return baseElement;
}

void Connector::setBaseElement(std::shared_ptr<Element> const & baseElement)
{
  this->baseElement = baseElement;
}

std::shared_ptr<Node> const & Connector::getOtherElement() const
{
  return otherElement;
}

void Connector::setOtherElement(std::shared_ptr<Node> const & otherElement)
{
  this->otherElement = otherElement;
}

float Connector::getBaseElementX() const
{
  return baseElementX;
}

void Connector::setBaseElementX(float baseElementX)
{
  this->baseElementX = baseElementX;
}

float Connector::getBaseElementBalance() const
{
  return baseElementBalance;
}

void Connector::setBaseElementBalance(float baseElementBalance)
{
  this->baseElementBalance = baseElementBalance;
}

float Connector::getBaseElementY() const
{
  return baseElementY;
}

void Connector::setBaseElementY(float baseElementY)
{
  this->baseElementY = baseElementY;
}

float Connector::getOtherElementX() const
{
  return otherElementX;
}

void Connector::setOtherElementX(float otherElementX)
{
  this->otherElementX = otherElementX;
  otherElement->setX(otherElementX);
}

float Connector::getOtherElementY() const
{
  return otherElementY;
}

void Connector::setOtherElementY(float otherElementY)
{
  this->otherElementY = otherElementY;
}

float Connector::getOtherElementBalance() const
{
  return otherElementBalance;
}

void Connector::setOtherElementBalance(float otherElementBalance)
{
  this->otherElementBalance = otherElementBalance;
}

bool Connector::getIsReversed() const
{
  return isReversed;
}

void Connector::setIsReversed(bool const isReversed)
{
  this->isReversed = isReversed;
}

std::list<std::pair<float, float>> const & Connector::getBendPoints() const
{
  return bendPoints;
}

std::shared_ptr<Element> Connector::getSourceElement() const
{
  return isReversed ? getOtherElement() : getBaseElement();
}

std::string Connector::getSourceElementIdForConnector() const
{
  return isReversed ? getOtherElement()->getIdForOtherElement() : getBaseElement()->getIdForBaseElement();
}

float Connector::getSourceElementX() const
{
  return isReversed ? getOtherElementX() : getBaseElementX();
}

float Connector::getSourceElementY() const
{
  return isReversed ? getOtherElementY() : getBaseElementY();
}

float Connector::getSourceElementBalance() const
{
  return isReversed ? getOtherElementBalance() : getBaseElementBalance();
}

std::shared_ptr<Element> Connector::getTargetElement() const
{
  return isReversed ? getBaseElement() : getOtherElement();
}

std::string Connector::getTargetElementIdForConnector() const
{
  return isReversed ? getBaseElement()->getIdForBaseElement() : getOtherElement()->getIdForOtherElement();
}

float Connector::getTargetElementX() const
{
  return isReversed ? getBaseElementX() : getOtherElementX();
}

float Connector::getTargetElementY() const
{
  return isReversed ? getBaseElementY() : getOtherElementY();
}

float Connector::getTargetElementBalance() const
{
  return isReversed ? getBaseElementBalance() : getOtherElementBalance();
}
}  // namespace formatTranslators
