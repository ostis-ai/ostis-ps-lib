#include "link.hpp"

#include "visitor/visitor.hpp"

namespace formatTranslators
{
void Link::acceptVisitor(Visitor & visitor)
{
  visitor.visitLink(*this);
}

Link::Link()
  : Node()
{
}

std::string const & Link::getContent() const
{
  return content;
}

void Link::setContent(std::string const & content)
{
  this->content = content;
}
}  // namespace formatTranslators
