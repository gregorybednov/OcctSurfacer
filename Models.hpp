#ifndef MODELS_HPP_
#define MODELS_HPP_
#include <string>
#include <vector>

#include <TopoDS_Face.hxx>
#include "./Relation.hpp"


struct Model {
  std::string filename;
  std::vector<TopoDS_Face> faces;
};

struct pnt {
  size_t i;
  size_t j;
};

struct result {
  struct pnt x;
  struct pnt y;
  Relation res;
};

bool operator==(struct pnt a, struct pnt b);
bool operator!=(struct pnt a, struct pnt b);

struct pntHash {
  std::size_t operator()(const pnt &p) const {
    return std::hash<size_t>()(p.i) ^ (std::hash<size_t>()(p.j) << 1);
  }
};

bool ckFaceHasAnyOtherRels(const std::vector<struct result> &results,
                           const pnt f);
std::vector<struct Model> readFiles(const std::vector<std::string> &names);
std::string facename(std::vector<struct Model> m, struct pnt p);
std::string st(const std::string &c, const std::string &a,
               const std::string &b);
std::string statements(struct result r, std::vector<struct Model> faces);

#endif  // MODELS_HPP_
