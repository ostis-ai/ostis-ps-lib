#include "link.hpp"

#include "visitor/visitor.hpp"

namespace formatTranslators
{
void Link::AcceptVisitor(Visitor & visitor)
{
  visitor.VisitLink(*this);
}

Link::Link() {}

std::string const & Link::GetContent() const
{
  return content;
}

void Link::SetContent(std::string const & content)
{
  this->content = content;
}

std::shared_ptr<Node> Link::Copy() const
{
  std::shared_ptr<Link> link = std::make_shared<Link>();
  Copy(*link);
  return link;
}

void Link::Copy(Link & copiedLink) const
{
  Node::Copy(copiedLink);
  copiedLink.SetContent(GetContent());
}
}  // namespace formatTranslators
