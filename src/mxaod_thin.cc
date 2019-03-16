#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <type_traits>
#include <algorithm>
#include <numeric>

#include <boost/regex.hpp>

#include <TFile.h>
#include <TTree.h>
#include <TKey.h>
#include <TLorentzVector.h>

#include "ivanp/timed_counter.hh"
#include "ivanp/string.hh"
#include "ivanp/program_options.hh"
#include "ivanp/root/branch_reader.hh"

#include "any_reader.hh"
#include "color.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;
using std::cerr;
using ivanp::cat;
using ivanp::error;

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

template <typename C, typename T>
unsigned find(const C& c, const T& x) {
  const auto _begin = begin(c);
  const auto _end = end(c);
  const auto it = std::find(_begin,_end,x);
  if (it==_end) throw error(x," not found");
  return std::distance(_begin,it);
}

int main(int argc, char* argv[]) {
  std::vector<const char*> ifnames;
  const char *ofname = nullptr, *branches = "branches.re";
  bool store_all = false, write_mom = false;

  try {
    using namespace ivanp::po;
    if (program_options()
      (ifnames,'i',"input MxAODs",req(),pos())
      (ofname,'o',"output file",req())
      (branches,'b',cat("file with regexes to match branches [",branches,']'))
      (store_all,"--all","also store events with isPassed!=true")
      (write_mom,"--mom","write 4-momentum vectors")
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

  std::vector<branch_reader<std::vector<float>>> _photons, _jets;
  std::vector<branch_reader<float>> _pT_y;
  std::array<std::vector<float>,4> photons, jets;
  for (auto& v : photons) v.resize(2);
  std::vector<unsigned> ph_i(2), jet_i;

  for (const char* ifname : ifnames) {
    cout << iftty{"\033[35m"} << ifname << iftty{"\033[0m"} << endl;
    lumi += get_lumi(ifname);

    TFile file(ifname);
    reader.SetTree("CollectionTree",&file);

    if (ifname == ifnames.front()) {
      std::vector<const TLeaf*> leaves;
      for (const auto& re : get_branch_regex(branches)) {
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

      if (write_mom) {
        tout->Branch("HGamPhotonsAuxDyn.pt" , &photons[0]);
        tout->Branch("HGamPhotonsAuxDyn.eta", &photons[1]);
        tout->Branch("HGamPhotonsAuxDyn.phi", &photons[2]);
        tout->Branch("HGamPhotonsAuxDyn.m"  , &photons[3]);

        tout->Branch("HGamAntiKt4EMTopoJetsAuxDyn.pt" , &jets[0]);
        tout->Branch("HGamAntiKt4EMTopoJetsAuxDyn.eta", &jets[1]);
        tout->Branch("HGamAntiKt4EMTopoJetsAuxDyn.phi", &jets[2]);
        tout->Branch("HGamAntiKt4EMTopoJetsAuxDyn.m"  , &jets[3]);

        _photons = {
          {reader,"HGamPhotonsAuxDyn.pt"},
          {reader,"HGamPhotonsAuxDyn.eta"},
          {reader,"HGamPhotonsAuxDyn.phi"},
          {reader,"HGamPhotonsAuxDyn.m"}
        };
        _jets = {
          {reader,"HGamAntiKt4EMTopoJetsAuxDyn.pt"},
          {reader,"HGamAntiKt4EMTopoJetsAuxDyn.eta"},
          {reader,"HGamAntiKt4EMTopoJetsAuxDyn.phi"},
          {reader,"HGamAntiKt4EMTopoJetsAuxDyn.m"}
        };
        _pT_y = {
          {reader,"HGamEventInfoAuxDyn.pT_y1"},
          {reader,"HGamEventInfoAuxDyn.pT_y2"}
        };
      }

      fout.cd();

      for (auto* key : *file.GetListOfKeys()) { // copy histograms
        if (
          ivanp::starts_with(key->GetName(),"CutFlow_") &&
          !ivanp::starts_with(key->GetName()+8,"Run")
        )
          static_cast<TKey*>(key)->ReadObj()->Clone();
      }
    }

    any_reader* isPassed = nullptr;
    if (!store_all) for (auto& var : vars) {
      if (ivanp::ends_with(var.name(),".isPassed")) {
        isPassed = &var;
        break;
      }
    }

    using tc = ivanp::timed_counter<Long64_t>;
    for (tc ent(reader.GetEntries(true)); reader.Next(); ++ent) {
      if (isPassed && !isPassed->get<Char_t>()) continue;
      for (auto& var : vars) *var;

      if (write_mom) {
        const auto& _photon_pt = *_photons[0];
        auto pT_y1 = *_pT_y[0], pT_y2 = *_pT_y[1];
        if (pT_y1 < pT_y2) std::swap(pT_y1,pT_y2);
        try {
          ph_i = { find(_photon_pt,pT_y1), find(_photon_pt,pT_y2) };
          if (ph_i[0]==ph_i[1]) throw error("same index");
        } catch (const std::exception& e) {
          cerr << "Photons: " << e << '\n';
          return 1;
        }

        for (size_t j=0; j<4; ++j)
          for (size_t i=0; i<2; ++i)
            photons[j][i] = (*_photons[j])[ph_i[i]];

        const auto& _jet_pt = *_jets[0];
        jet_i.resize(_jet_pt.size());
        if (jet_i.size()) {
          std::iota(jet_i.begin(), jet_i.end(), 0);
          std::sort(jet_i.begin(), jet_i.end(), [&](auto a, auto b){
            return _jet_pt[a] > _jet_pt[b];
          });
          while (jet_i.size() && _jet_pt[jet_i.back()] < 30e3)
            jet_i.pop_back();
        }

        const size_t njets = jet_i.size();
        for (size_t j=0; j<4; ++j) {
          jets[j].resize(njets);
          for (size_t i=0; i<njets; ++i)
            jets[j][i] = (*_jets[j])[jet_i[i]];
        }
      }

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

