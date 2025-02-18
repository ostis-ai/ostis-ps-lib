#pragma once

#include "node.hpp"

namespace formatTranslators
{
class Link : public Node
{
public:
  Link();

  std::string const & GetContent() const override;

  void SetContent(std::string const & content);

  void AcceptVisitor(Visitor & visitor) override;

  std::shared_ptr<Node> Copy() const override;

private:
  std::string content;

  void Copy(Link & copiedLink) const;
};

}  // namespace formatTranslators
