#pragma once

#include "element.hpp"

namespace formatTranslators
{
class Node : public Element
{
public:
  Node();

  void AcceptVisitor(Visitor & visitor) override;

  virtual std::string const & GetContent() const;

  float GetX() const;

  void SetX(float x);

  float GetTopY() const;

  void SetTopY(float topY);

  float GetBottomY() const;

  void SetBottomY(float bottomY);

  bool HasBus() const;

  std::string GetIdForBaseElement() const override;

  std::string GetIdForOtherElement() const override;

  std::string GetNodeId() const;

  std::string GetBusId() const;

  virtual std::shared_ptr<Node> Copy() const;

protected:
  void Copy(Node & copiedNode) const;

private:
  float x;
  float topY;
  float bottomY;
};

}  // namespace formatTranslators
