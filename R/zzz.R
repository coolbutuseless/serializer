
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# This code was stolen from the 'archive' package
# https://github.com/r-lib/archive/blob/main/R/zzz.R
#
# this will dynamically load the tiny library of code which interfaces directly 
# to R_GetConnection(), R_ReadConnection() and R_WriteConnection()
#
# By storing this interface in this library outside of the actual main 
# *.so for this package, it is hidden from R CMD check!
#
# this is Sneak Level 1000!
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.onLoad <- function(libname, pkgname) {
  lib_path <- system.file("lib", .Platform$r_arch, paste0("libconnection", .Platform$dynlib.ext), package = "archive")
  res <- dyn.load(lib_path)
  
  rchive_init(res$new_connection$address, res$read_connection$address)
}

.onUnload <- function(libname) {
  lib_path <- system.file("lib", .Platform$r_arch, paste0("libconnection", .Platform$dynlib.ext), package = "archive")
  dyn.unload(lib_path)
}
