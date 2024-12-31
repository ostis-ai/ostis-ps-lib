#include <sc-memory/utils/sc_log.hpp>
#include <sc-memory/sc_debug.hpp>
#include "gwf_translator_visitor.hpp"

#include "model/connector.hpp"
#include "model/element.hpp"
#include "model/link.hpp"
#include "model/node.hpp"

namespace formatTranslators
{
struct GWFTranslatorVisitor::ScTypeHash
{
  sc_type operator()(ScType const & type) const
  {
    return type;
  }
};

std::unordered_map<ScType, std::string, GWFTranslatorVisitor::ScTypeHash> const GWFTranslatorVisitor::nodeTypes = {
    {ScType::Node, "node/-/-/not_define"},

    {ScType::ConstNode, "node/const/perm/general"},
    {ScType::ConstNodeMaterial, "node/const/perm/terminal"},
    {ScType::ConstNodeStructure, "node/const/perm/struct"},
    {ScType::ConstNodeTuple, "node/const/perm/tuple"},
    {ScType::ConstNodeRole, "node/const/perm/role"},
    {ScType::ConstNodeNonRole, "node/const/perm/relation"},
    {ScType::ConstNodeClass, "node/const/perm/group"},
    {ScType::ConstNodeSuperclass, "node/const/perm/super_group"},
    {ScType::ConstNodeLink, "node/const/perm/general"},
    {ScType::ConstNodeLinkClass, "node/const/perm/general"},

    {ScType::VarNode, "node/var/perm/general"},
    {ScType::VarNodeMaterial, "node/var/perm/terminal"},
    {ScType::VarNodeStructure, "node/var/perm/struct"},
    {ScType::VarNodeTuple, "node/var/perm/tuple"},
    {ScType::VarNodeRole, "node/var/perm/role"},
    {ScType::VarNodeRole, "node/var/perm/attribute"},
    {ScType::VarNodeNonRole, "node/var/perm/relation"},
    {ScType::VarNodeClass, "node/var/perm/group"},
    {ScType::VarNodeSuperclass, "node/var/perm/super_group"},
    {ScType::VarNodeLink, "node/var/perm/general"},
    {ScType::VarNodeLinkClass, "node/var/perm/general"},
};

std::unordered_map<ScType, std::string, GWFTranslatorVisitor::ScTypeHash> const GWFTranslatorVisitor::connectorTypes = {
    {ScType::CommonEdge, "pair/-/-/-/noorient"},
    {ScType::CommonArc, "pair/-/-/-/orient"},

    {ScType::ConstCommonEdge, "pair/const/-/perm/noorien"},
    {ScType::ConstCommonArc, "pair/const/-/perm/orient"},
    {ScType::ConstPermPosArc, "pair/const/pos/perm/orient/membership"},
    {ScType::ConstTempPosArc, "pair/const/pos/temp/orient/membership"},
    {ScType::ConstPermNegArc, "pair/const/neg/perm/orient/membership"},
    {ScType::ConstTempNegArc, "pair/const/neg/temp/orient/membership"},
    {ScType::ConstFuzArc, "pair/const/fuz/perm/orient/membership"},

    {ScType::VarCommonEdge, "pair/var/-/perm/noorien"},
    {ScType::VarCommonArc, "pair/var/-/perm/orient"},
    {ScType::VarPermPosArc, "pair/var/pos/perm/orient/membership"},
    {ScType::VarTempPosArc, "pair/var/pos/temp/orient/membership"},
    {ScType::VarPermNegArc, "pair/var/neg/perm/orient/membership"},
    {ScType::VarTempNegArc, "pair/var/neg/temp/orient/membership"},
    {ScType::VarFuzArc, "pair/var/fuz/perm/orient/membership"},
};

std::string GWFTranslatorVisitor::getStaticSectorContent() const
{
  return staticSectorContent.str();
}

void GWFTranslatorVisitor::visitConnector(Connector & connector)
{
  connector.getOtherElement()->acceptVisitor(*this);
  staticSectorContent << "        <pair type=\"" << getGWFConnectorType(connector) << "\" idtf=\""
                      << connector.getIdentifier() << "\" shapeColor=\"0\" id=\"" << getGWFId(connector)
                      << "\" parent=\"0\" id_b=\"" << connector.getSourceElementIdForConnector() << "\" id_e=\""
                      << connector.getTargetElementIdForConnector() << "\" b_x=\"" << connector.getSourceElementX()
                      << "\" b_y=\"" << connector.getSourceElementY() << "\" e_x=\"" << connector.getTargetElementX()
                      << "\" e_y=\"" << connector.getTargetElementY() << "\" dotBBalance=\""
                      << connector.getSourceElementBalance() << "\" dotEBalance=\""
                      << connector.getTargetElementBalance() << "\">\n";
  auto const & bendPoints = connector.getBendPoints();
  if (bendPoints.empty())
  {
    staticSectorContent << "          <points/>\n";
  }
  else
  {
    for (auto const & coordinates : bendPoints)
      staticSectorContent << "                <point x=\"" << coordinates.first << "\" y=\"" << coordinates.second
                          << "\"/>\n";
  }
  staticSectorContent << "        </pair>\n";
  visitIncidentConnectors(connector);
}

void GWFTranslatorVisitor::visitLink(Link & link)
{
  bool haveBus = link.hasBus();
  staticSectorContent << "        <node type=\"" << getGWFNodeType(link) << "\" idtf=\"" << link.getIdentifier()
                      << "\" shapeColor=\"0\" id=\"" << getGWFId(link) << "\" parent=\"0\" x=\"" << link.getX()
                      << "\" y=\"" << link.getTopY() << "\" haveBus=\"" << (haveBus ? "true" : "false")
                      << "\" idtf_pos=\"" << static_cast<int>(link.getIdentifierPosition())
                      << "\">\n"
                         "          <content type=\""
                      << getLinkContentType(link) << "\" mime_type=\"" << getLinkMimeType(link)
                      << "\" content_visibility=\"true\" file_name=\"\"><![CDATA[" << link.getContent()
                      << "]]></content>\n"
                         "        </node>\n";
  if (haveBus)
    addBus(link);
  visitIncidentConnectors(link);
}

void GWFTranslatorVisitor::visitNode(Node & node)
{
  bool haveBus = node.hasBus();
  std::string identifier = node.getIdentifier();
  staticSectorContent
      << "        <node type=\"" << getGWFNodeType(node) << "\" idtf=\"" << identifier << "\" shapeColor=\"0\" id=\""
      << getGWFId(node) << "\" parent=\"0\" x=\"" << node.getX() << "\" y=\"" << node.getTopY() << "\" haveBus=\""
      << (haveBus ? "true" : "false") << "\" idtf_pos=\"" << static_cast<int>(node.getIdentifierPosition()) << "\">\n"
      << "            <content type=\"0\" mime_type=\"\" content_visibility=\"false\" file_name=\"\"/>\n"
         "        </node>\n";
  if (haveBus)
    addBus(node);
  visitIncidentConnectors(node);
}

std::string GWFTranslatorVisitor::getGWFId(Element & element)
{
  std::string id = element.getId();
  if (id.empty())
  {
    id = std::to_string(nextIdToAssign++);
    element.setId(id);
  }
  return id;
}

std::string GWFTranslatorVisitor::getGWFConnectorType(Connector const & connector) const
{
  auto const & type = connectorTypes.find(connector.getScType());
  if (type == connectorTypes.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType,
        "GWFTranslatorVisitor: cannot convert connector type " << std::string(connector.getScType()) << " to gwf");
  return type->second;
}

std::string GWFTranslatorVisitor::getGWFNodeType(Node const & node) const
{
  auto const & type = nodeTypes.find(node.getScType());
  if (type == nodeTypes.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType,
        "GWFTranslatorVisitor: cannot convert node type " << std::string(node.getScType()) << " to gwf");
  return type->second;
}

std::string GWFTranslatorVisitor::getLinkMimeType(Link const & link) const
{
  return "content/term";
}

std::string GWFTranslatorVisitor::getLinkContentType(Link const & link) const
{
  return "1";
}

void GWFTranslatorVisitor::visitIncidentConnectors(Element const & element)
{
  for (auto const & connector : element.getConnectors())
    connector->acceptVisitor(*this);
}

void GWFTranslatorVisitor::addBus(Node const & node)
{
  staticSectorContent << "        <bus type=\"bus\" idtf=\"\" shapeColor=\"0\" id=\"" << node.getBusId()
                      << "\" parent=\"0\" owner=\"" << node.getNodeId() << "\" b_x=\"" << node.getX() << "\" b_y=\""
                      << node.getTopY() << "\" e_x=\"" << node.getX() << "\" e_y=\"" << node.getBottomY()
                      << "\">\n"
                         "            <points/>\n"
                         "        </bus>\n";
}
}  // namespace formatTranslators
