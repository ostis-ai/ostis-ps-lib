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
  void AcceptVisitor(Visitor & visitor) override;

  std::shared_ptr<Element> const & GetBaseElement() const;

  void SetBaseElement(std::shared_ptr<Element> const & baseElement);

  std::shared_ptr<Node> const & GetOtherElement() const;

  void SetOtherElement(std::shared_ptr<Node> const & otherElement);

  float GetBaseElementX() const;

  void SetBaseElementX(float baseElementX);

  float GetBaseElementBalance() const;

  void SetBaseElementBalance(float baseElementBalance);

  float GetBaseElementY() const;

  void SetBaseElementY(float baseElementY);

  float GetOtherElementX() const;

  void SetOtherElementX(float otherElementX);

  float GetOtherElementY() const;

  void SetOtherElementY(float otherElementY);

  float GetOtherElementBalance() const;

  void SetOtherElementBalance(float otherElementBalance);

  bool GetIsReversed() const;

  void SetIsReversed(bool const isReversed);

  std::list<std::pair<float, float>> const & GetBendPoints() const;

  std::shared_ptr<Element> GetSourceElement() const;

  std::string GetSourceElementIdForConnector() const;

  float GetSourceElementX() const;

  float GetSourceElementY() const;

  float GetSourceElementBalance() const;

  std::shared_ptr<Element> GetTargetElement() const;

  std::string GetTargetElementIdForConnector() const;

  float GetTargetElementX() const;

  float GetTargetElementY() const;

  float GetTargetElementBalance() const;

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
