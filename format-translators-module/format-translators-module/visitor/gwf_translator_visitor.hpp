#pragma once

#include <sstream>
#include <unordered_map>

#include <sc-memory/sc_type.hpp>

#include "visitor.hpp"

namespace formatTranslators
{
class Element;

class GWFTranslatorVisitor : public Visitor
{
public:
  std::string GetStaticSectorContent() const;

  void VisitConnector(Connector & connector) override;

  void VisitLink(Link & link) override;

  void VisitNode(Node & node) override;

private:
  void VisitIncidentConnectors(Element const & element);

  void AddBus(Node const & node);

  std::string GetGWFId(Element & element);

  std::string GetGWFConnectorType(Connector const & connector) const;

  std::string GetGWFNodeType(Node const & node) const;

  std::string GetLinkMimeType(Link const & link) const;

  std::string GetLinkContentType(Link const & link) const;

  std::stringstream staticSectorContent;

  struct ScTypeHash;
  static std::unordered_map<ScType, std::string, ScTypeHash> const NODE_TYPES;
  static std::unordered_map<ScType, std::string, ScTypeHash> const CONNECTOR_TYPES;
  uint32_t nextIdToAssign = 0;
};

}  // namespace formatTranslators
