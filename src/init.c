
#include <R.h>
#include <Rinternals.h>

extern SEXP unmarshall_(SEXP raw_vec_);
extern SEXP calc_serialized_size_(SEXP robj_);
extern SEXP marshall_(SEXP robj_);
extern SEXP marshall_con_(SEXP robj_, SEXP con_);
extern SEXP unmarshall_con_(SEXP con_);

static const R_CallMethodDef CEntries[] = {

  {"unmarshall_"      , (DL_FUNC) &unmarshall_       , 1},
  {"calc_serialized_size_", (DL_FUNC) &calc_serialized_size_ , 1},
  {"marshall_"        , (DL_FUNC) &marshall_         , 1},
  {"marshall_con_"    , (DL_FUNC) &marshall_con_     , 2},
  {"unmarshall_con_"  , (DL_FUNC) &unmarshall_con_   , 1},
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



