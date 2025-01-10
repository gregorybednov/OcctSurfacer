#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include "./Models.hpp"
#include "./Relation.hpp"

std::vector<struct Model> readFiles(const std::vector<std::string> &names) {
  std::vector<struct Model> result;

  for (auto name : names) {
    struct Model tmp;
    tmp.filename = name;
    tmp.faces = std::vector<TopoDS_Face>();

    TopoDS_Shape shape;
    BRep_Builder builder;

    if (!BRepTools::Read(shape, name.c_str(), builder)) {
      std::cerr << "Error: " << name << " not opened\n";
      continue;
    }

    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next()) {
      tmp.faces.push_back(TopoDS::Face(exp.Current()));
    }
    result.push_back(tmp);
  }
  return result;
}

bool operator==(struct pnt a, struct pnt b) { return a.i == b.i && a.j == b.j; }

bool operator!=(struct pnt a, struct pnt b) { return a.i != b.i || a.j != b.j; }

bool ckFaceHasAnyOtherRels(const std::vector<struct result> &results,
                           const pnt f) {

  std::unordered_set<size_t> all_filecodes, where_exists;
  for (auto r: results) {
    if (f.i == r.x.i) continue; // forget about the points from the same file
    if (f.i == r.y.i) continue; // forget about the points from the same file

    auto oth = (r.x == f) ? r.y : r.x;
    all_filecodes.insert(oth.i);
    if (r.res == Relation::Equals
    || r.res == Relation::Included && r.x == f
    || r.res == Relation::Includes && r.y == f) {
      where_exists.insert(oth.i);
      continue;
    }
    if (r.res != Relation::Irrelates) {
      return true;
    }
  }
  return all_filecodes.size() > where_exists.size();
}
  //std::unordered_set<pnt, pntHash> relates, whereExists;
  //for (auto r : results) {
 //   if (r.x != f && r.y != f) {
 //     continue;
 //   }
 //   auto oth = (r.x == f) ? r.y : r.x;
 //   if (r.res != Relation::Irrelates) {
 //     relates.insert(oth);
 //   }
 //   if (r.res == Relation::Equals
 //   || r.res == Relation::Includes && f == r.y
 //   || r.res == Relation::Included && f == r.x) {
  //    whereExists.insert(oth);
  //  }
 // }
 // return relates.size() > whereExists.size();
//}

std::string st(const std::string &c, const std::string &a,
               const std::string &b) {
  return c + " \"" + a + "\" \"" + b + "\"\n";
}

std::string facename(std::vector<struct Model> m, struct pnt p) {
  return m[p.i].filename + "/" + std::to_string(p.j + 1);
}

std::string statements(struct result r, std::vector<struct Model> faces) {
  auto A = facename(faces, r.x);
  auto B = facename(faces, r.y);
  switch (r.res) {
  case Relation::Negates:
  case Relation::Crosses:
    return st("E", A, B);
  case Relation::Equals:
    return st("A", A, B ) + st("A", B, A);
  case Relation::Includes:
    return st("A", A, B);
  case Relation::Included:
    return st("A", B, A);
  case Relation::Intersects:
    return st("I", A, B);
  }
  return "";
}
