
#include <R.h>
#include <Rinternals.h>

extern SEXP calc_serialized_size_(SEXP robj_);

extern SEXP marshall_raw_(SEXP robj_);
extern SEXP unmarshall_raw_(SEXP raw_vec_);

extern SEXP marshall_file_(SEXP robj_, SEXP filename_);
extern SEXP unmarshall_file_(SEXP filename_);

extern SEXP marshall_con_(SEXP robj_, SEXP con_);
extern SEXP unmarshall_con_(SEXP con_);

extern SEXP marshall_con_illegal_(SEXP robj_, SEXP con_);
extern SEXP unmarshall_con_illegal_(SEXP con_);

static const R_CallMethodDef CEntries[] = {
  
  {"calc_serialized_size_", (DL_FUNC) &calc_serialized_size_ , 1},
  
  {"marshall_raw_"        , (DL_FUNC) &marshall_raw_         , 1},
  {"unmarshall_raw_"      , (DL_FUNC) &unmarshall_raw_       , 1},
  
  {"marshall_file_"       , (DL_FUNC) &marshall_file_        , 2},
  {"unmarshall_file_"     , (DL_FUNC) &unmarshall_file_      , 1},
  
  {"marshall_con_"        , (DL_FUNC) &marshall_con_         , 2},
  {"unmarshall_con_"      , (DL_FUNC) &unmarshall_con_       , 1},
  
  {"marshall_con_illegal_"  , (DL_FUNC) &marshall_con_illegal_  , 2},
  {"unmarshall_con_illegal_", (DL_FUNC) &unmarshall_con_illegal_, 1},
  {NULL , NULL, 0}
};


void R_init_serializer(DllInfo *info) {
  R_registerRoutines(
    info,      // DllInfo
    NULL,      // .C
    CEntries,  // .Call
    NULL,      // Fortran
    NULL       // External
  );
  R_useDynamicSymbols(info, FALSE);
}



