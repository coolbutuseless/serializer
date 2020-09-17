
#include <R.h>
#include <Rinternals.h>

extern SEXP pack_();
extern SEXP unpack_();

static const R_CallMethodDef CEntries[] = {

  {"pack_"  , (DL_FUNC) &pack_  , 1},
  {"unpack_", (DL_FUNC) &unpack_, 1},
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



