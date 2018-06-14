#ifndef ANY_READER_HH
#define ANY_READER_HH

#include <algorithm>

#include <TTree.h>
#include <TLeaf.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "ivanp/enum_traits.hh"
#include "ivanp/error.hh"

#define ROOT_TYPES \
(Float_t)(Double_t)(Int_t)(UInt_t)(Char_t)(UChar_t)(Long64_t)(ULong64_t)(Short_t)(UShort_t)(Bool_t)

MAKE_ENUM(types,ROOT_TYPES)

class any_reader {
  types type;
  bool set = false;

#define ANY_READER_EACH BOOST_PP_SEQ_FOR_EACH(ANY_READER_EACH_OP,,ROOT_TYPES)

#define ANY_READER_EACH_OP(r,_,E) , sizeof(E)
  mutable char value_mem[ std::max({ 1ul ANY_READER_EACH }) ];
#undef ANY_READER_EACH_OP

#define ANY_READER_EACH_OP(r,_,E) , sizeof(TTreeReaderValue<E>)
  mutable char reader_mem[ std::max({ 1ul ANY_READER_EACH }) ];
#undef ANY_READER_EACH_OP

  template <typename T>
  inline T* value_ptr() const noexcept {
    void* mem = value_mem;
    return static_cast<T*>(mem);
  }

  template <typename T>
  inline TTreeReaderValue<T>* reader_ptr() const noexcept {
    void* mem = reader_mem;
    return static_cast<TTreeReaderValue<T>*>(mem);
  }

public:
  any_reader(TTreeReader& reader, const TLeaf* leaf) {
    const char* type_name = leaf->GetTypeName();
    const char* name = leaf->GetName();
    try {
      type = enum_traits<types>::val(type_name);
    } catch (const std::exception& e) {
      throw ivanp::error("any_reader for \'",type_name,' ',name,"\'\n",e.what());
    }
#define ANY_READER_EACH_OP(r,_,E) \
  case types::E: new(reader_mem) TTreeReaderValue<E>(reader,name); break;

    switch (type) { ANY_READER_EACH }

#undef ANY_READER_EACH_OP
    set = true;
  }

  any_reader(TTreeReader& reader, const char* name)
  : any_reader(reader,reader.GetTree()->GetLeaf(name)) { }

  ~any_reader() {
    if (set) {
#define ANY_READER_EACH_OP(r,_,E) \
  case types::E: reader_ptr<E>()->~TTreeReaderValue<E>(); break;

      switch (type) { ANY_READER_EACH }

#undef ANY_READER_EACH_OP
    }
  }

  void operator*() {
#define ANY_READER_EACH_OP(r,_,E) \
  case types::E: *value_ptr<E>() = **reader_ptr<E>(); break;

    switch (type) { ANY_READER_EACH }

#undef ANY_READER_EACH_OP
  }

  const char* name() const {
#define ANY_READER_EACH_OP(r,_,E) \
  case types::E: return reader_ptr<E>()->GetBranchName();

    switch (type) { ANY_READER_EACH }
    return nullptr;

#undef ANY_READER_EACH_OP
  }

  types get_type() const noexcept { return type; }

  void make_branch(TTree* tout) const {
#define ANY_READER_EACH_OP(r,_,E) \
  case types::E: \
    tout->Branch( reader_ptr<E>()->GetBranchName(), value_ptr<E>() ); break;

    switch (type) { ANY_READER_EACH }

#undef ANY_READER_EACH_OP
  }

  template <typename T>
  inline T get() { return **reader_ptr<T>(); }

  template <typename F>
  friend inline auto operator%(any_reader& reader, F&& f) {
#define ANY_READER_EACH_OP(r,_,E) \
  case types::E: return f(*reader.value_ptr<E>());

    switch (reader.type) { ANY_READER_EACH }

#undef ANY_READER_EACH_OP
  }
};

#endif
