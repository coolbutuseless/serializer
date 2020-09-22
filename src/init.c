
#include <R.h>
#include <Rinternals.h>

extern SEXP marshall_();
extern SEXP unmarshall_();

static const R_CallMethodDef CEntries[] = {

  {"marshall_"  , (DL_FUNC) &marshall_  , 1},
  {"unmarshall_", (DL_FUNC) &unmarshall_, 1},
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



