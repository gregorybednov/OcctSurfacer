#include <string>
#include <unordered_map>
#include <vector>

#include "./Relation.hpp"

#define FOR_EACH(x, type, y)                                                   \
  for (TopExp_Explorer x(y, TopAbs_##type); x.More(); x.Next())

std::string toString(const Relation value) {
  const std::unordered_map<Relation, std::string> strings = {
      {Relation::Irrelates, "Irrelates"}, {Relation::Crosses, "Crosses"},
      {Relation::Includes, "Includes"},   {Relation::Included, "Included"},
      {Relation::Equals, "Equals"},       {Relation::Intersects, "Intersects"},
      {Relation::Negates, "Negates"}};
  return strings.at(value);
}

const Standard_Real EPSI = Precision::Confusion();

std::vector<TopoDS_Edge> getEdges(const TopoDS_Face &f) {
  std::vector<TopoDS_Edge> res;
  FOR_EACH(e, EDGE, f) { res.push_back(TopoDS::Edge(e.Current())); }
  return res;
}

bool ckSection(const TopoDS_Face &f1, const TopoDS_Face &f2) {
  TopoDS_Shape section = BRepAlgoAPI_Section(f1, f2);
  if (section.IsNull())
    return false;
  const std::vector<TopoDS_Edge> v1 = getEdges(f1);
  const std::vector<TopoDS_Edge> v2 = getEdges(f2);
  auto edgesFromFaces(v1);
  edgesFromFaces.insert(edgesFromFaces.end(), v2.begin(), v2.end());
  FOR_EACH(edges, COMPOUND, section) {
    FOR_EACH(edge, EDGE, edges.Current()) {
      TopoDS_Shape e = edge.Current();
      for (auto c : edgesFromFaces) {
        if (e.IsNull() || c.IsNull())
          continue;
        e = BRepAlgoAPI_Cut(e, c);
      }
      if (e.NbChildren() > 0)
        return true;
    }
  }
  return false;
}

bool normalsAreCodirected(const TopoDS_Face &f1, const TopoDS_Face &f2) {
  Handle(Geom_Surface) s1 = BRep_Tool::Surface(f1);
  Handle(Geom_Surface) s2 = BRep_Tool::Surface(f2);

  // it may not work on complicated surfaces!!!
  const gp_Pnt p = s1->Value(0.5, 0.5);

  GeomLProp_SLProps props1(s1, p.X(), p.Y(), 1, EPSI);
  GeomLProp_SLProps props2(s2, p.X(), p.Y(), 1, EPSI);
  if (!props1.IsNormalDefined() || !props2.IsNormalDefined())
    return false;
  gp_Dir n1 = props1.Normal();
  gp_Dir n2 = props2.Normal();
  Standard_Real dotProduct = n1.Dot(n2);
  return std::abs(dotProduct - 1.0) < EPSI;
}

bool ckCommon(const TopoDS_Face &face1, const TopoDS_Face &face2) {
  TopoDS_Shape common = BRepAlgoAPI_Common(face1, face2);
  return !common.IsNull() && TopExp_Explorer(common, TopAbs_FACE).More();
}

bool isEmpty(const TopoDS_Shape &f1) {
  GProp_GProps props;
  BRepGProp::SurfaceProperties(f1, props);
  return props.Mass() < EPSI;
}

// WARNING: DID you read returning type? it's not face but shape
TopoDS_Shape operator-(const TopoDS_Face &x, const TopoDS_Face &y) {
  return TopExp_Explorer(BRepAlgoAPI_Cut(x, y), TopAbs_FACE).Current();
}

// WARNING: DOES NOT check has it any face in common or not, use ckCommon() !
TopoDS_Face operator&(const TopoDS_Face &x, const TopoDS_Face &y) {
  return TopoDS::Face(
      TopExp_Explorer(BRepAlgoAPI_Common(x, y), TopAbs_FACE).Current());
}

Relation determine(const TopoDS_Face &face1, const TopoDS_Face &face2) {
  if (!ckCommon(face1, face2)) {
    if (ckSection(face1, face2))
      return Relation::Crosses;
    return Relation::Irrelates;
  }

  if (!normalsAreCodirected(face1 & face2, face2 & face1))
    return Relation::Negates;

  if (isEmpty(face1 - face2) && isEmpty(face2 - face1))
    return Relation::Equals;

  if (!isEmpty(face1 - face2) && !isEmpty(face2 - face1))
    return Relation::Intersects;  // TODO(greg): mb should go a little deeper..

  if (isEmpty(face1 - face2))
    return Relation::Included;
  else
    return Relation::Includes;
}
