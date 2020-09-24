
#include <R.h>
#include <Rinternals.h>

extern SEXP marshall_();
extern SEXP unmarshall_();
extern SEXP calc_size_robust_();
extern SEXP calc_size_fast_();
extern SEXP marshall_fast_();

static const R_CallMethodDef CEntries[] = {

  {"marshall_"        , (DL_FUNC) &marshall_            , 1},
  {"unmarshall_"      , (DL_FUNC) &unmarshall_          , 1},
  {"calc_size_robust_", (DL_FUNC) &calc_size_robust_    , 1},
  {"calc_size_fast_"  , (DL_FUNC) &calc_size_fast_      , 1},
  {"marshall_fast_"   , (DL_FUNC) &marshall_fast_       , 2},
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



