#include <iostream>
#include <tuple>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>

#include "ivanp/timed_counter.hh"
#include "ivanp/string.hh"
#include "ivanp/program_options.hh"
#include "ivanp/root/tkey.hh"
#include "ivanp/error.hh"

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
    std::cerr << e << endl;
    return 1;
  }

  std::string files_str;

  TFile fout(ofname,"recreate");
  if (fout.IsZombie()) return 1;
  // TTree* tout = new TTree("CollectionTree","");
  TTree* tout = nullptr;

  std::vector<TH1*> cutflow;

  Float_t weight;
  const char* weight_name = "HGamEventInfoAuxDyn.weight";

  bool first_file = true;
  for (const auto& ifname : ifnames) {
    const double scale = get<1>(ifname);
    cout << get<0>(ifname) << "  " << scale << endl;
    TFile fin(get<0>(ifname).c_str());

    TTree *tin;
    fin.GetObject("CollectionTree",tin);
    // tin->SetMakeClass(1);

    // for (auto* obj : *tin->GetListOfBranches()) {
    //   TBranch* b = static_cast<TBranch*>(obj);
    //   if (!strcmp(b->GetName(),"HGamEventInfoAuxDyn.weight")) {
    //     if (first_file)
    //       tout->Branch(b->GetName(),&weight);
    //     b->SetAddress(&weight);
    //   } else {
    //     b->SetTree(tout);
    //   }
    // }


    if (first_file) {
      first_file = false;
      fout.cd();

      tout = tin->CloneTree(0);
      tin ->SetBranchAddress(weight_name,&weight);
      tout->SetBranchAddress(weight_name,&weight);

      for (auto& key : get_keys(&fin)) {
        const char* name = key.GetName();
        if (ivanp::starts_with(name,"CutFlow_")) {
          TH1* h = static_cast<TH1*>(key_cast<TH1>(key)->Clone());
          h->Scale(scale);
          cutflow.push_back(h);
        }
      }

    } else {
      tin->CopyAddresses(tout);
      tin ->SetBranchAddress(weight_name,&weight);
      tout->SetBranchAddress(weight_name,&weight);

      for (TH1* h : cutflow) {
        const char* name = h->GetName();
        TH1* h1 = dynamic_cast<TH1*>(fin.Get(name));
        if (!h1) throw std::runtime_error(cat(
          "missing \"",name,"\" in file ",fin.GetName()));
        h->Add(h1,scale);
      }
    }

    using tc = ivanp::timed_counter<Long64_t>;
    for (tc ent(tin->GetEntriesFast()); !!ent; ++ent) {
      tin->GetEntry(ent);
      weight *= scale;
      tout->Fill();
    }

    files_str += cat(scale,":\n",fin.Get("Files")->GetTitle(),'\n');
  }
  fout.cd();

  write("Files",files_str);

  fout.Write(0,TObject::kOverwrite);

  cout << "output: " << fout.GetName() << endl;
}
