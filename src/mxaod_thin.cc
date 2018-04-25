#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <type_traits>

#include <boost/regex.hpp>

#include <TFile.h>
#include <TTree.h>

#include "any_reader.hh"
#include "timed_counter.hh"
#include "color.hh"
#include "string.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;
using ivanp::cat;

// namespace ivanp {
// template <>
// inline size_t size(const TObjArray& c) { return c.GetSize(); }
// }

template <typename... T>
inline void write(const char* name, T&&... x) {
  TNamed(name,cat(std::forward<T>(x)...).c_str()).Write();
}

double get_lumi(const char* name) {
  static const boost::regex re("^(?:.*/)?data.*_(\\d+)ipb\\..*\\.root$");
  boost::cmatch match;
  if (boost::regex_match(name,name+strlen(name),match,re)) {
    return std::stod(match[1]);
  } else throw std::runtime_error("lumi regex failed to match");
}

TTree *tout;
TTreeReader reader;

boost::regex alias_re("[^.]*Event[^.]*\\.(.+)");

std::vector<boost::regex> get_branch_regex(const char* fname) {
  std::vector<boost::regex> re;
  std::ifstream f(fname);
  std::string line;
  const char *a, *b;
  while (std::getline(f,line)) {
    if (line.empty()) continue;
    a = line.c_str();
    while (std::isspace(*a)) ++a;
    if ((*a)=='\0' || (*a)=='#') continue;
    b = a + line.size() - 1;
    while (std::isspace(*b)) --b;
    // auto str = "(?:[^.]*\\.)?"+std::string(a,b+1);
    // TEST(str)
    re.emplace_back("(?:[^.]*\\.)?"+std::string(a,b+1));
  }
  return re;
}

int main(int argc, char* argv[]) {
  if (argc<3) {
    cout << "usage: " << argv[0] << " out.root in.root" << endl;
    return 1;
  }

  TFile fout(argv[1],"recreate"/*,"mxaod_thin",209*/);
  if (fout.IsZombie()) return 1;
  tout = new TTree("CollectionTree","");

  double lumi = 0; // ipb
  std::string fnames;
  std::vector<any_reader> vars; // can't move

  for (int i=2; i<argc; ++i) {
    cout << iftty{"\033[35m"} << argv[i] << iftty{"\033[0m"} << endl;
    lumi += get_lumi(argv[i]);

    TFile file(argv[i]);
    reader.SetTree("CollectionTree",&file);

    if (i==2) {
      std::vector<const TLeaf*> leaves;
      for (const auto& re : get_branch_regex("branches.re")) {
        for (const auto* obj : *reader.GetTree()->GetListOfLeaves()) {
          const TLeaf* leaf = static_cast<const TLeaf*>(obj);
          const char* name = leaf->GetName();
          if (!boost::regex_match(name,re)) continue;
          leaves.push_back(leaf);
        }
      }
      vars.reserve(leaves.size()); // can't move
      for (const auto* leaf : leaves) {
        const char* name = leaf->GetName();
        vars.emplace_back(reader,leaf); // can't move
        const auto& var = vars.back();
        var.make_branch(tout);
        cout << var.get_type() << ' ' << var.name();
        // add alias
        boost::cmatch match;
        if (boost::regex_match(name,name+std::strlen(name),match,alias_re)) {
          std::string alias = match[1];
          if (!alias.empty()) {
            const auto _30 = alias.find("_30");
            if (_30!=std::string::npos) alias.erase(_30,3);
            tout->SetAlias(alias.c_str(),name);
            cout << " -> " << alias;
          }
        }
        cout << endl;
      }
    } else fnames += '\n';
    fnames += argv[i];

    using tc = ivanp::timed_counter<Long64_t>;
    for (tc ent(reader.GetEntries(true)); reader.Next(); ++ent) {
      for (auto& var : vars) *var;
      tout->Fill();
    }
  }
  fout.cd();

  write("Lumi",lumi," ipb");
  cout << iftty{"\033[36m"} << "Total lumi" << iftty{"\033[0m"}
       << ": " << lumi << " ipb" << endl;
  write("Files",fnames);

  fout.Write(0,TObject::kOverwrite);
}

