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

std::shared_ptr<Node> Link::copy() const
{
  std::shared_ptr<Link> link = std::make_shared<Link>();
  copy(*link);
  return link;
}

void Link::copy(Link & copiedLink) const
{
  Node::copy(copiedLink);
  copiedLink.setContent(getContent());
}
}  // namespace formatTranslators
