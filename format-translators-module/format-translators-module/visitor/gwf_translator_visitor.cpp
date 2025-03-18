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

std::unordered_map<ScType, std::string, GWFTranslatorVisitor::ScTypeHash> const GWFTranslatorVisitor::NODE_TYPES = {
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

std::unordered_map<ScType, std::string, GWFTranslatorVisitor::ScTypeHash> const GWFTranslatorVisitor::CONNECTOR_TYPES =
    {
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

std::string GWFTranslatorVisitor::GetStaticSectorContent() const
{
  return staticSectorContent.str();
}

void GWFTranslatorVisitor::VisitConnector(Connector & connector)
{
  connector.GetOtherElement()->AcceptVisitor(*this);
  staticSectorContent << "        <pair type=\"" << GetGWFConnectorType(connector) << "\" idtf=\""
                      << connector.GetIdentifier() << "\" shapeColor=\"0\" id=\"" << GetGWFId(connector)
                      << "\" parent=\"0\" id_b=\"" << connector.GetSourceElementIdForConnector() << "\" id_e=\""
                      << connector.GetTargetElementIdForConnector() << "\" dotBBalance=\""
                      << connector.GetSourceElementBalance() << "\" dotEBalance=\""
                      << connector.GetTargetElementBalance() << "\" sc_addr=\"" << connector.GetScAddress().Hash()
                      << "\">\n";
  auto const & bendPoints = connector.GetBendPoints();
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
  VisitIncidentConnectors(connector);
}

void GWFTranslatorVisitor::VisitLink(Link & link)
{
  bool haveBus = link.HasBus();
  staticSectorContent << "        <node type=\"" << GetGWFNodeType(link) << "\" idtf=\"" << link.GetIdentifier()
                      << "\" shapeColor=\"0\" id=\"" << GetGWFId(link) << "\" parent=\"0\" x=\"" << link.GetX()
                      << "\" y=\"" << link.GetTopY() << "\" haveBus=\"" << (haveBus ? "true" : "false")
                      << "\" idtf_pos=\"" << static_cast<int>(link.GetIdentifierPosition()) << "\" sc_addr=\""
                      << link.GetScAddress().Hash()
                      << "\">\n"
                         "          <content type=\""
                      << GetLinkContentType(link) << "\" mime_type=\"" << GetLinkMimeType(link)
                      << "\" content_visibility=\"true\" file_name=\"\"><![CDATA[" << link.GetContent()
                      << "]]></content>\n"
                         "        </node>\n";
  if (haveBus)
    AddBus(link);
  VisitIncidentConnectors(link);
}

void GWFTranslatorVisitor::VisitNode(Node & node)
{
  bool haveBus = node.HasBus();
  std::string identifier = node.GetIdentifier();
  staticSectorContent
      << "        <node type=\"" << GetGWFNodeType(node) << "\" idtf=\"" << identifier << "\" shapeColor=\"0\" id=\""
      << GetGWFId(node) << "\" parent=\"0\" x=\"" << node.GetX() << "\" y=\"" << node.GetTopY() << "\" haveBus=\""
      << (haveBus ? "true" : "false") << "\" idtf_pos=\"" << static_cast<int>(node.GetIdentifierPosition())
      << "\"  sc_addr=\"" << node.GetScAddress().Hash() << "\">\n"
      << "            <content type=\"0\" mime_type=\"\" content_visibility=\"false\" file_name=\"\"/>\n"
         "        </node>\n";
  if (haveBus)
    AddBus(node);
  VisitIncidentConnectors(node);
}

void GWFTranslatorVisitor::VisitIncidentConnectors(Element const & element)
{
  for (auto const & connector : element.GetConnectors())
    connector->AcceptVisitor(*this);
}

void GWFTranslatorVisitor::AddBus(Node const & node)
{
  staticSectorContent << "        <bus type=\"bus\" idtf=\"\" shapeColor=\"0\" id=\"" << node.GetBusId()
                      << "\" parent=\"0\" owner=\"" << node.GetNodeId() << "\" b_x=\"" << node.GetX() << "\" b_y=\""
                      << node.GetTopY() << "\" e_x=\"" << node.GetX() << "\" e_y=\"" << node.GetBottomY()
                      << "\">\n"
                         "            <points/>\n"
                         "        </bus>\n";
}

std::string GWFTranslatorVisitor::GetGWFId(Element & element)
{
  std::string id = element.GetId();
  if (id.empty())
  {
    id = std::to_string(nextIdToAssign++);
    element.SetId(id);
  }
  return id;
}

std::string GWFTranslatorVisitor::GetGWFConnectorType(Connector const & connector) const
{
  auto const & type = CONNECTOR_TYPES.find(connector.GetScType());
  if (type == CONNECTOR_TYPES.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType,
        "GWFTranslatorVisitor: cannot convert connector type " << std::string(connector.GetScType()) << " to gwf");
  return type->second;
}

std::string GWFTranslatorVisitor::GetGWFNodeType(Node const & node) const
{
  auto const & type = NODE_TYPES.find(node.GetScType());
  if (type == NODE_TYPES.cend())
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidType,
        "GWFTranslatorVisitor: cannot convert node type " << std::string(node.GetScType()) << " to gwf");
  return type->second;
}

std::string GWFTranslatorVisitor::GetLinkMimeType(Link const & link) const
{
  // TODO(kilativ-dotcom): support other mime types
  return "content/term";
}

std::string GWFTranslatorVisitor::GetLinkContentType(Link const & link) const
{
  // TODO(kilativ-dotcom): support other content types
  return "1";
}
}  // namespace formatTranslators
