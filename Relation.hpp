#ifndef RELATION_HPP_DEFINED
#define RELATION_HPP_DEFINED

#include <vector>
#include <string>

#include <Standard_Real.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <GeomLProp_SLProps.hxx>
#include <BRep_Tool.hxx>

enum class Relation {
  Irrelates,
  Crosses,
  Includes,
  Included,
  Equals,
  Intersects,
  Negates
};

std::string toString(const Relation value);

Relation determine(const TopoDS_Face &face1, const TopoDS_Face &face2);

#endif  // RELATION_HPP_
