.onLoad <- function(libname, pkgname) {
  lib_path <- system.file("lib", .Platform$r_arch, paste0("libconnection", .Platform$dynlib.ext), package = "serializer")
  res <- dyn.load(lib_path)

  init_smuggle(res$read_connection$address, res$write_connection$address)
}

.onUnload <- function(libname) {
  lib_path <- system.file("lib", .Platform$r_arch, paste0("libconnection", .Platform$dynlib.ext), package = "serializer")
  dyn.unload(lib_path)
}

