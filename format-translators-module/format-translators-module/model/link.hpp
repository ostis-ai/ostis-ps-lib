#pragma once

#include "node.hpp"

namespace formatTranslators
{
class Link : public Node
{
public:
  Link();

  void AcceptVisitor(Visitor & visitor) override;

  std::string const & GetContent() const override;

  void SetContent(std::string const & content);

  std::shared_ptr<Node> Copy() const override;

private:
  std::string content;

  void Copy(Link & copiedLink) const;
};

}  // namespace formatTranslators
