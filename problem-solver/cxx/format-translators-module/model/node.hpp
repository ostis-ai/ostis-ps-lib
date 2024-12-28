#pragma once

#include "element.hpp"

namespace formatTranslators
{
class Node : public Element
{
public:
  Node();

  float getX() const;

  void setX(float x);

  float getTopY() const;

  void setTopY(float topY);

  float getBottomY() const;

  void setBottomY(float bottomY);

  bool hasBus() const;

  std::string getIdForBaseElement() const override;

  std::string getIdForOtherElement() const override;

  std::string getNodeId() const;

  std::string getBusId() const;

  size_t calculateReservedVerticalElementsOnParent();

  void acceptVisitor(Visitor & visitor) override;

private:
  float x;
  float topY;
  float bottomY;
  size_t reservedVerticalElementsOnParent = 0;
};

}  // namespace formatTranslators
