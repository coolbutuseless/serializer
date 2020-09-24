

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


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Serialize an R object to a raw vector, while minimising memory allocations
#'
#' This is a slightly more complex version of \code{marshall()} which
#' pre-calculates the size of the serialized representation.  This means there
#' only needs to be 1 memory allocation (not including those internal to R),
#' and gives a speed boost for medium-to-large objects
#'
#' @param robj R object
#' @param fast use fast method for pre-calculation of size.
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall_fast <- function(robj, fast = FALSE) {
  .Call(marshall_fast_, robj, fast)
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Calculate the size of the R object when it is serialized
#'
#' This function goes through all the motions of serializing an object, but
#' does nothing with the bytes other than to tally the total length.
#'
#' @param robj R object
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
calc_size_robust <- function(robj) {
  .Call(calc_size_robust_, robj)
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Calculate the approximate size of the R object when it is serialized
#'
#' This function returns a size which is greater-than-or-equal-to that returned by
#' \code{calc_size_robust}.
#'
#' This function is faster than \code{calc_size_robust()} but has little support
#' for language and function objects.  If unsupported language objects are found
#' then this function will return a negative value.
#'
#' @param robj R object
#'
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
calc_size_fast <- function(robj) {
  .Call(calc_size_fast_, robj)
}
