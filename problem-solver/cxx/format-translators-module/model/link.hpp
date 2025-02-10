#pragma once

#include "node.hpp"

namespace formatTranslators
{
class Link : public Node
{
public:
  Link();

  std::string const & getContent() const override;

  void setContent(std::string const & content);

  void acceptVisitor(Visitor & visitor) override;

  std::shared_ptr<Node> copy() const override;

private:
  std::string content;

  void copy(Link & copiedLink) const;
};

}  // namespace formatTranslators
