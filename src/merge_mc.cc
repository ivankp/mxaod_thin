#include <iostream>
#include <tuple>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include "ivanp/timed_counter.hh"
#include "ivanp/string.hh"
#include "ivanp/program_options.hh"
#include "ivanp/root/tkey.hh"

#include "any_reader.hh"
#include "color.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;
using std::get;
using ivanp::cat;

template <typename... T>
inline void write(const char* name, T&&... x) {
  TNamed(name,cat(std::forward<T>(x)...).c_str()).Write();
}

int main(int argc, char* argv[]) {
  std::vector<std::tuple<std::string,double>> ifnames;
  const char *ofname = nullptr;

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

  TFile fout(ofname,"recreate");
  if (fout.IsZombie()) return 1;
  TTree* tout = new TTree("CollectionTree","");

  std::string files_str;

  TTreeReader reader;
  std::vector<any_reader> vars; // can't move
  any_reader* weight = nullptr;

  std::vector<TH1*> cutflow;

  bool first_file = true;
  for (const auto& ifname : ifnames) {
    const double scale = get<1>(ifname);
    cout << get<0>(ifname) << "  " << scale << endl;
    TFile fin(get<0>(ifname).c_str());

    reader.SetTree("CollectionTree",&fin);

    if (first_file) {
      std::vector<const TLeaf*> leaves;
      for (const auto* obj : *reader.GetTree()->GetListOfLeaves()) {
        leaves.push_back(static_cast<const TLeaf*>(obj));
      }

      vars.reserve(leaves.size()); // can't move
      for (const auto* leaf : leaves) {
        vars.emplace_back(reader,leaf); // can't move
        const auto& var = vars.back();
        var.make_branch(tout);
        // cout << var.get_type() << ' ' << var.name() << endl;
      }

      for (auto& var : vars) {
        if (ivanp::ends_with(var.name(),".weight")) {
          weight = &var;
          break;
        }
      }
      if (!weight) throw std::runtime_error("no branch \"weight\"");

      fout.cd();
      for (auto& key : get_keys(&fin)) {
        const char* name = key.GetName();
        if (ivanp::starts_with(name,"CutFlow_")) {
          TH1* h = static_cast<TH1*>(key_cast<TH1>(key)->Clone());
          h->Scale(scale);
          cutflow.push_back(h);
        }
      }

      first_file = false;
    } else {
      for (TH1* h : cutflow) {
        const char* name = h->GetName();
        TH1* h1 = dynamic_cast<TH1*>(fin.Get(name));
        if (!h1) throw std::runtime_error(cat(
          "missing \"",name,"\" in file ",fin.GetName()));
        h->Add(h1,scale);
      }
    }

    using tc = ivanp::timed_counter<Long64_t>;
    for (tc ent(reader.GetEntries(true)); reader.Next(); ++ent) {
      for (auto& var : vars) *var;
      (*weight) % [=](auto& x){ x *= scale; };
      tout->Fill();
    }

    files_str += cat(scale,":\n",fin.Get("Files")->GetTitle(),'\n');
  }
  fout.cd();

  write("Files",files_str);

  fout.Write(0,TObject::kOverwrite);

  cout << "output: " << fout.GetName() << endl;
}
