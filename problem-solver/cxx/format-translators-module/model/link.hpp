#pragma once

#include "node.hpp"

namespace formatTranslators
{
class Link : public Node
{
public:
  Link();

  std::string const & getContent() const;

  void setContent(std::string const & content);

  void acceptVisitor(Visitor & visitor) override;

private:
  std::string content;
};

}  // namespace formatTranslators
