
#include <R.h>
#include <Rinternals.h>

extern SEXP marshall_();
extern SEXP unmarshall_();
extern SEXP calc_marshalled_size_();
extern SEXP marshall_minimize_malloc_();

static const R_CallMethodDef CEntries[] = {

  {"marshall_"  , (DL_FUNC) &marshall_  , 1},
  {"unmarshall_", (DL_FUNC) &unmarshall_, 1},
  {"calc_marshalled_size_", (DL_FUNC) &calc_marshalled_size_, 1},
  {"marshall_minimize_malloc_", (DL_FUNC) &marshall_minimize_malloc_, 1},
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



