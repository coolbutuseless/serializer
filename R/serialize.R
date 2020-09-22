

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Serialize/unserialize an R object to/from a raw vector.
#'
#' This is just a demonstration package showing how serialization may be done
#' directly in C, rather than using \code{base::serialize()}.
#'
#' @param robj R object
#' @param raw_vec R object serialized as a raw vector
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall <- function(robj) {
  .Call(marshall_, robj)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' @rdname marshall
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall <- function(raw_vec) {
  .Call(unmarshall_, raw_vec)
}
