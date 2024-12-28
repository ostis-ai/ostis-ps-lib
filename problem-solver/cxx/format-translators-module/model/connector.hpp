#pragma once

#include "element.hpp"
#include "node.hpp"

#include <list>
#include <memory>

namespace formatTranslators
{
class Connector : public Element
{
public:
  std::shared_ptr<Element> const & getBaseElement() const;

  void setBaseElement(std::shared_ptr<Element> const & baseElement);

  std::shared_ptr<Node> const & getOtherElement() const;

  void setOtherElement(std::shared_ptr<Node> const & otherElement);

  float getBaseElementX() const;

  void setBaseElementX(float baseElementX);

  float getBaseElementY() const;

  void setBaseElementY(float baseElementY);

  float getBaseElementBalance() const;

  void setBaseElementBalance(float baseElementBalance);

  float getOtherElementX() const;

  void setOtherElementX(float otherElementX);

  float getOtherElementY() const;

  void setOtherElementY(float otherElementY);

  float getOtherElementBalance() const;

  void setOtherElementBalance(float otherElementBalance);

  bool getIsReversed() const;

  void setIsReversed(bool const isReversed);

  std::list<std::pair<float, float>> const & getBendPoints() const;

  std::shared_ptr<Element> getSourceElement() const;

  std::string getSourceElementIdForConnector() const;

  float getSourceElementX() const;

  float getSourceElementY() const;

  float getSourceElementBalance() const;

  std::shared_ptr<Element> getTargetElement() const;

  std::string getTargetElementIdForConnector() const;

  float getTargetElementX() const;

  float getTargetElementY() const;

  float getTargetElementBalance() const;

  void acceptVisitor(Visitor & visitor) override;

private:
  std::shared_ptr<Element> baseElement;
  std::shared_ptr<Node> otherElement;
  float baseElementX;
  float baseElementY;
  float baseElementBalance;
  float otherElementX;
  float otherElementY;
  float otherElementBalance;
  bool isReversed;
  std::list<std::pair<float, float>> bendPoints;
};

}  // namespace formatTranslators
