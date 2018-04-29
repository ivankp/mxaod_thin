#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <type_traits>

#include <boost/regex.hpp>

#include <TFile.h>
#include <TTree.h>
#include <TKey.h>

#include "any_reader.hh"
#include "color.hh"
#include "ivanp/timed_counter.hh"
#include "ivanp/string.hh"
#include "ivanp/program_options.hh"

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
  } return 0;
}

TTree *tout;
TTreeReader reader;

const boost::regex alias_re("([^.]*)Event[^.]*\\.(.+)");

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
    re.emplace_back("(?:[^.]*\\.)?"+std::string(a,b+1));
  }
  return re;
}

int main(int argc, char* argv[]) {
  std::vector<const char*> ifnames;
  const char* ofname = nullptr;

  try {
    using namespace ivanp::po;
    if (program_options()
      (ifnames,'i',"input MxAODs",req(),pos())
      (ofname,'o',"output file",req())
      .parse(argc,argv,true)) return 0;
  } catch (const std::exception& e) {
    std::cerr << iftty{"\033[31m"} << e.what() << iftty{"\033[0m"} << endl;
    return 1;
  }

  TFile fout(ofname,"recreate"/*,"mxaod_thin",209*/);
  if (fout.IsZombie()) return 1;
  tout = new TTree("CollectionTree","");

  double lumi = 0; // ipb
  std::vector<any_reader> vars; // can't move

  for (const char* ifname : ifnames) {
    cout << iftty{"\033[35m"} << ifname << iftty{"\033[0m"} << endl;
    lumi += get_lumi(ifname);

    TFile file(ifname);
    reader.SetTree("CollectionTree",&file);

    if (ifname == ifnames.front()) {
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
          std::string alias = match[2];
          if (!alias.empty()) {
            const auto _30 = alias.find("_30");
            if (_30!=std::string::npos) alias.erase(_30,3);
            if (ivanp::ends_with(match[1],"Truth"))
              alias = "truth." + alias;
            tout->SetAlias(alias.c_str(),name);
            cout << " -> " << alias;
          }
        }
        cout << endl;
      }

      fout.cd();

      for (auto* key : *file.GetListOfKeys()) {
        if (ivanp::starts_with(key->GetName(),"CutFlow_"))
          static_cast<TKey*>(key)->ReadObj()->Clone();
      }

    }

    using tc = ivanp::timed_counter<Long64_t>;
    for (tc ent(reader.GetEntries(true)); reader.Next(); ++ent) {
      for (auto& var : vars) *var;
      tout->Fill();
    }
  }
  fout.cd();

  if (lumi) {
    write("Lumi",lumi," ipb");
    cout << iftty{"\033[36m"} << "Total lumi" << iftty{"\033[0m"}
         << ": " << lumi << " ipb" << endl;
  }
  write("Files",ivanp::lcat(ifnames,"\n"));

  fout.Write(0,TObject::kOverwrite);
}

