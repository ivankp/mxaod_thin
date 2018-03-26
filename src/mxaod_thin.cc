#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <type_traits>

#include <boost/regex.hpp>

#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
// #include <TTreeReaderArray.h>

#include "timed_counter.hh"
#include "color.hh"

using namespace std;

template <typename... T>
inline void write(const char* name, T&&... x) {
  stringstream ss;
  using expander = int[];
  (void)expander{0, ((void)(ss << forward<T>(x)), 0)...};
  TNamed(name,ss.str().c_str()).Write();
}

double get_lumi(const char* name) {
  static const boost::regex re("^(.*/)?data.*_(\\d+)ipb\\..*\\.root$");
  boost::cmatch match;
  if (boost::regex_match(name,name+strlen(name),match,re)) {
    return stod(match[2]);
  } else throw runtime_error("lumi regex failed to match");
}

TTree *tout;
TTreeReader reader;

struct basic_var {
  virtual void operator*() = 0;
  virtual ~basic_var() { }
};

template <typename T>
struct var_wrap final: basic_var {
  TTreeReaderValue<T> r;
  T x;
  var_wrap(const char* name): r(reader,name) {
    tout->Branch(name,&x);

    static const char* suff[] = { "HGamEventInfoAuxDyn", "EventInfoAux" };
    const char* dot = strchr(name,'.');
    if (!dot) return;
    int match = 0;
    string alias;
    for (unsigned i=0; i<std::extent<decltype(suff)>::value; ++i) {
      for (const char *a=name, *b=suff[i]; a!=dot && *b!='\0'; ++a, ++b)
        if (*a != *b) goto next;
      match = i+1;
      alias = dot+1;
      break;
next: ;
    }

    if (match) {
      if (match==1) {
        const auto _30 = alias.find("_30");
        if (_30!=string::npos) alias.erase(_30,3);
      }
      tout->SetAlias(alias.c_str(),name);
    }
  }
  void operator*() { x = *r; }
};

vector<basic_var*> vars;

template <typename T>
void var(const char* name) { vars.push_back(new var_wrap<T>(name)); }

int main(int argc, char* argv[]) {
  if (argc!=3) {
    cout << "usage: " << argv[0] << " out.root in.root" << endl;
    return 1;
  }

  TFile fout(argv[1],"recreate"/*,"mxaod_thin",209*/);
  if (fout.IsZombie()) return 1;
  tout = new TTree("CollectionTree","");

  var<UInt_t>("EventInfoAux.runNumber");
  var<ULong64_t>("EventInfoAux.eventNumber");
  var<Char_t>("HGamEventInfoAuxDyn.isPassed");

#define VAR(NAME) var<Float_t>("HGamEventInfoAuxDyn." #NAME);

  VAR(m_yy) VAR(pT_yy) VAR(yAbs_yy) VAR(cosTS_yy) VAR(pTt_yy)
  VAR(Dy_y_y)

  var<Int_t>("HGamEventInfoAuxDyn.N_j_30");
  var<Int_t>("HGamEventInfoAuxDyn.N_j_50");

  VAR(HT_30)
  VAR(pT_j1_30)      VAR(pT_j2_30)      VAR(pT_j3_30)
  VAR(yAbs_j1_30)    VAR(yAbs_j2_30)
  VAR(Dphi_j_j_30)   VAR(Dphi_j_j_30_signed)
  VAR(Dy_j_j_30)     VAR(m_jj_30)
  VAR(sumTau_yyj_30) VAR(maxTau_yyj_30)
  VAR(pT_yyjj_30)    VAR(Dy_yy_jj_30) VAR(Dphi_yy_jj_30)

  var<Int_t>("HGamEventInfoAuxDyn.N_lep_15");
  var<Int_t>("HGamEventInfoAuxDyn.catCoup_Moriond2017BDT");
  var<Char_t>("HGamEventInfoAuxDyn.catXS_VBF");
  var<Char_t>("HGamEventInfoAuxDyn.catXS_ttH");

  VAR(met_TST)

#define VEC(NAME) \
  var<vector<float>>(#NAME ".pt"); \
  var<vector<float>>(#NAME ".eta"); \
  var<vector<float>>(#NAME ".phi"); \
  var<vector<float>>(#NAME ".m");

  VEC(HGamPhotonsAuxDyn)
  VEC(HGamAntiKt4EMTopoJetsAuxDyn)
  VEC(HGamAntiKt4EMPFlowJetsAuxDyn)

  double lumi = 0; // ipb

  for (int i=2; i<argc; ++i) {
    cout << iftty{"\033[35m"} << argv[i] << iftty{"\033[0m"} << endl;
    lumi += get_lumi(argv[i]);

    TFile file(argv[i]);
    reader.SetTree("CollectionTree",&file);

    using tc = ivanp::timed_counter<Long64_t>;
    for (tc ent(reader.GetEntries(true)); reader.Next(); ++ent) {
      for (auto* v : vars) **v;
      tout->Fill();
    }
  }
  fout.cd();

  write("Lumi",lumi," ipb");
  cout << iftty{"\033[36m"} << "Total lumi" << iftty{"\033[0m"}
       << ": " << lumi << " ipb" << endl;

  fout.Write(0,TObject::kOverwrite);

  for (auto* v : vars) delete v;
}

