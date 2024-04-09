


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Serialize an R object to a raw vector or connection.
#'
#' @param robj R object
#' @param con connection to write the data
#'
#' @return If \code{con} is a connection, return nothing.  Otherwise, return 
#'         a raw vector containing the serialized data.
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall(mtcars, con = gzfile(tmp))
#' unmarshall(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall <- function(robj, con = NULL) {
  if (is.null(con)) {
    .Call(marshall_, robj)
  } else {
    stopifnot(inherits(con, "connection"))
    if(!isOpen(con)){
      on.exit(close(con)) 
      open(con, "wb")
    }
    invisible(.Call(marshall_con_, robj, con))
  }
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Unserialize an R object from a raw vector or connection
#' 
#' @param x A raw vector containing serialized data or a connection 
#' 
#' @return Return the unserialized data
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall(mtcars, con = gzfile(tmp))
#' unmarshall(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall <- function(x) {
  if (inherits(x, "connection")) {
    con <- x
    if(!isOpen(con)){
      on.exit(close(con)) 
      open(con, "rb")
    }
    .Call(unmarshall_con_, con)
  } else {
    .Call(unmarshall_, x)
  }
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Calculate the size of the R object when it is serialized
#'
#' This function goes through all the motions of serializing an object, but
#' does nothing with the bytes other than to tally the total length.
#'
#' @param robj R object
#' @return number of bytes needed to serialize this object
#'
#' @export
#' 
#' @examples
#' calc_serialized_size(mtcars)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
calc_serialized_size <- function(robj) {
  .Call(calc_serialized_size_, robj)
}

