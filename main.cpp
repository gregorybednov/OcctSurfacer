#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "./Models.hpp"
#include "./Relation.hpp"

#ifdef ENABLE_OPENMP
#include <omp.h>
#else
inline void omp_set_num_threads(int) {}
inline int omp_get_num_threads() { return 1; }
inline int omp_get_thread_num() { return 0; }
#endif

bool showIrrelates = false;
bool showHelp = false;
bool showRelations = false;

void cli(int argc, char *argv[]) {
  if (argc > 1) {
    showRelations = argv[1] == std::string("-named");
    showIrrelates = argc > 2 && argv[2] == std::string("-irrelates");
    showHelp = argv[1] == std::string("--help") ||
               argv[1] == std::string("-h") || argv[1] == std::string("-help");
  }

  std::vector<std::string> helpMsg = {
      "This program calculates relations between different faces in B-REP CAD "
      "(OpenCascade) format.",
      "",
      "You should put filenames into STDIN and wait for the result in STDOUT "
      "or error messages in STDERR.",
      "You should end your input with EOF symbol (in console, call Ctrl-D for "
      "Linux/macOS or Ctrl-Z for Windows)",
      "",
      "Availiable cli options: ",
      " -named - displays named relations (Equals, Includes, etc.) instead of "
      "AIEO traditional logic statements"
      " -irrelates - required to show Irrelates between Faces; must be after "
      "-named ONLY",
      " -h, -help, --help - to show this help message",
      "",
      "Also program puts a message \"smth added to files list.\" into STDERR",
      "every time you add something"};

  if (showHelp) {
    for (auto s : helpMsg) {
      std::cout << s << "\n";
    }
    exit(0);
  }
}

int main(int argc, char *argv[]) {
  cli(argc, argv);

  std::string str;
  std::vector<std::string> filenames;
  while (getline(std::cin, str)) {
    filenames.push_back(str);
    std::cerr << str << " added to files list.\n";
  }
  std::vector<size_t> nums;
  auto faces = readFiles(filenames);

  std::vector<struct result> result;
  const size_t limit = faces.size();

  #pragma omp parallel
  {
    std::vector<struct result> local_result;

    #pragma omp for
    for (size_t i = 0; i < limit; ++i) {
      for (size_t j = i + 1; j < limit; ++j) {
        for (size_t i0 = 0; i0 < faces[i].faces.size(); ++i0) {
          for (size_t j0 = 0; j0 < faces[j].faces.size(); ++j0) {
            struct result r;
            r.x.i = i;
            r.y.i = j;
            r.x.j = i0;
            r.y.j = j0;
            r.res = determine(faces[i].faces[i0], faces[j].faces[j0]);
            local_result.push_back(r);
          }
        }
      }
    }

    #pragma omp critical
    {
      result.insert(result.end(), local_result.begin(), local_result.end());
    }
  }

  std::vector<struct result> filtered;
  for (const auto& x : result) {
    if (ckFaceHasAnyOtherRels(result, x.x) &&
        ckFaceHasAnyOtherRels(result, x.y) &&
        x.res != Relation::Irrelates) {
      filtered.push_back(x);
    }
  }

  for (auto el : filtered) {
    if (!showIrrelates && showRelations && el.res == Relation::Irrelates)
      continue;
    if (showRelations) {
      std::cout << toString(el.res) << " \"" << facename(faces, el.x) << "\" \""
                << facename(faces, el.y) << "\"\n";
      continue;
    } else {
      std::cout << statements(el, faces);
    }
  }

  if (showRelations)
    exit(0);

  std::unordered_set<pnt, pntHash> facesInRelations;
  for (auto el : filtered) {
    facesInRelations.insert(el.x);
    facesInRelations.insert(el.y);
  }



  std::unordered_set<std::string> filesWithFiR;
  for (auto el : facesInRelations) {
    filesWithFiR.insert(faces[el.i].filename);
  }

  for (auto el : filesWithFiR) {
    std::cout << st("I", el, el);
    std::cout << st("O'", el, el);
  }

  for (auto el : facesInRelations) {
    std::cout << st("I", faces[el.i].filename, facename(faces, el));
    std::cout << st("I", facename(faces, el), facename(faces, el));
    std::cout << st("O'", facename(faces, el), facename(faces, el));
  }




  return 0;
}
