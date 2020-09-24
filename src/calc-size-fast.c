


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int calc_size_fast(SEXP s);



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int OutStringVec(SEXP s) {
  int count = 0;

  R_xlen_t len = XLENGTH(s);
  count += 4; // place holder
  count += len > R_SHORT_LEN_MAX  ? 12 : 4; // WriteLENGTH
  for (R_xlen_t i = 0; i < len; i++) {
    count += calc_size_fast(STRING_ELT(s, i));
  }
  return count;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This doesn't keep track of which objects are purely a reference object
// and will thus over-count. This is fine for its intended purpose
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int calc_size_fast(SEXP s) {
  int i;
  SEXP t;
  int count = 0;

  if (TRUE &&                         // R_compile_pkgs
      TYPEOF(s) == CLOSXP         &&
      TYPEOF(BODY(s)) != BCODESXP &&
      TRUE                        &&  // !R_disable_bytecode
      (!IS_S4_OBJECT(s) ||
        (!inherits(s, "refMethodDef") && !inherits(s, "defaultBindingFunction"))
      )
    ) {
    error("Unsupported object present. Try 'calc_size_fast()` instead");
  }

  tailcall:
    R_CheckStack();
  if (ALTREP(s)) {
    SEXP info = ALTREP_SERIALIZED_CLASS(s);
    SEXP state = ALTREP_SERIALIZED_STATE(s);
    if (info != NULL && state != NULL) {
      PROTECT(state);
      PROTECT(info);
      count += 4; // OutInteger(stream, flags);
      count += calc_size_fast(info);
      count += calc_size_fast(state);
      count += calc_size_fast(ATTRIB(s));
      UNPROTECT(2);
      return count;
    }
  }
  if (TYPEOF(s) == SYMSXP) {
    count += 4; // SYMSXP;
    count += calc_size_fast(PRINTNAME(s));
  } else if (TYPEOF(s) == ENVSXP) {
    if (R_IsPackageEnv(s)) {
      SEXP name = R_PackageEnvName(s);
      count += 4; // PACKAGESXP;
      count += OutStringVec(name);
    }
    else if (R_IsNamespaceEnv(s)) {
      count += 4; // NAMESPACESXP;
      count += OutStringVec(PROTECT(R_NamespaceEnvSpec(s)));
      UNPROTECT(1);
    }
    else {
      count += 4; // ENVSXP
      count += 4; // R_EnvironmentIsLocked(s)?
      count += calc_size_fast(ENCLOS(s));
      count += calc_size_fast(FRAME(s));
      count += calc_size_fast(HASHTAB(s));
      count += calc_size_fast(ATTRIB(s));
    }
  }
  else {
    int flags, hastag, hasattr;
    R_xlen_t len;
    switch(TYPEOF(s)) {
    case LISTSXP:
    case LANGSXP:
    case CLOSXP:
    case PROMSXP:
    case DOTSXP: hastag = TAG(s) != R_NilValue; break;
    default: hastag = FALSE;
    }
    hasattr = (TYPEOF(s) != CHARSXP && ATTRIB(s) != R_NilValue);
    count += 4; // Packed flags take up 1 integer
    switch (TYPEOF(s)) {
    case LISTSXP:
    case LANGSXP:
    case CLOSXP:
    case PROMSXP:
    case DOTSXP:
      // Dotted pair objects
      if (hasattr)
        count += calc_size_fast(ATTRIB(s));
      if (TAG(s) != R_NilValue)
        count += calc_size_fast(TAG(s));
      if (BNDCELL_TAG(s)) {
        error("Unsupported language object (R_expand_binding_value). Try `calc_size_robust()");
        // R_expand_binding_value(s);
      }
      count += calc_size_fast(CAR(s));
      s = CDR(s);
      goto tailcall;
    case EXTPTRSXP:
      count += calc_size_fast(EXTPTR_PROT(s));
      count += calc_size_fast(EXTPTR_TAG(s));
      break;
    case WEAKREFSXP:
      break;
    case SPECIALSXP:
    case BUILTINSXP:
      error("Unsupported language object (BUILTINSXP). Try `calc_size_robust()");
      break;
    case CHARSXP:
      if (s == NA_STRING)
        count += 4; // NA_String = -1L;
      else {
        count += 4; // LENGTH(s)
        count += LENGTH(s); // Character data
      }
      break;
    case LGLSXP:
    case INTSXP:
      len = XLENGTH(s);
      count += len > R_SHORT_LEN_MAX  ? 12 : 4; // WriteLENGTH;
      count += len * 4; // Integer data
      break;
    case REALSXP:
      len = XLENGTH(s);
      count += len > R_SHORT_LEN_MAX ? 12 : 4; // WriteLENGTH
      count += len * sizeof(double); // Double floating point data
      break;
    case CPLXSXP:
      len = XLENGTH(s);
      count += len > R_SHORT_LEN_MAX  ? 12 : 4; // WriteLENGTH
      count += len * sizeof(Rcomplex); // Complex data. Probably size = 2 * double
      break;
    case STRSXP:
      len = XLENGTH(s);
      count += len > R_SHORT_LEN_MAX  ? 12 : 4; // WriteLENGTH
      count += len * 4; // flags for each string
      count += len * 4; // write the length of each string
      SEXP *ps = STDVEC_DATAPTR(s);
      for (R_xlen_t ix = 0; ix < len; ix++) {
        if (ps[ix] != NA_STRING) {
          count += XLENGTH(ps[ix]);
        }
      }
      break;
    case VECSXP:
    case EXPRSXP:
      len = XLENGTH(s);
      count += len > R_SHORT_LEN_MAX  ? 12 : 4; // WriteLENGTH
      for (R_xlen_t ix = 0; ix < len; ix++)
        count += calc_size_fast(VECTOR_ELT(s, ix));
      break;
    case BCODESXP:
      error("Unsupported language object (BCODESXP). Try `calc_size_robust()");
      // WriteBC(s);
      break;
    case RAWSXP:
      len = XLENGTH(s);
      count += len > R_SHORT_LEN_MAX ? 12 : 4; // WriteLENGTH
      count += len;
      break;
    case S4SXP:
      break;
    }
    if (hasattr)
      count += calc_size_fast(ATTRIB(s));
  }

  return count;
}









int calc_size_fast_outer(SEXP s) {
  return calc_size_fast(s) + 23;
}










SEXP calc_size_fast_(SEXP s) {
  return ScalarInteger(calc_size_fast_outer(s));
}


