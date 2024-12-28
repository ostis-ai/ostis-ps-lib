#pragma once

#include <sstream>
#include <unordered_map>
#include <mutex>

#include <sc-memory/sc_type.hpp>

#include "visitor.hpp"

namespace formatTranslators
{
class Element;

class GWFTranslatorVisitor : public Visitor
{
public:
  std::string getStaticSectorContent() const;

  void visitConnector(Connector & connector) override;

  void visitLink(Link & link) override;

  void visitNode(Node & node) override;

private:
  void visitIncidentConnectors(Element const & element);

  void addBus(Node const & node);

  std::string getGWFId(Element & element);

  std::string getGWFConnectorType(Connector const & connector) const;

  std::string getGWFNodeType(Node const & node) const;

  std::string getLinkMimeType(Link const & link) const;

  std::string getLinkContentType(Link const & link) const;

  std::stringstream staticSectorContent;

  struct ScTypeHash;
  static std::unordered_map<ScType, std::string, ScTypeHash> const nodeTypes;
  static std::unordered_map<ScType, std::string, ScTypeHash> const connectorTypes;
  size_t nextIdToAssign = 0;
};

}  // namespace formatTranslators
