


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Serialize an R object to a raw vector 
#'
#' @param robj R object
#'
#' @return a raw vector containing the serialized data.
#' @export
#'
#' @examples
#' m <- marshall_raw(head(mtcars))
#' m
#' unmarshall_raw(m)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall_raw <- function(robj) {
  .Call(marshall_raw_, robj)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Serialize an R object to a connection
#' 
#' @inheritParams marshall_raw
#' @param con connection to write the data to
#'
#' @return \code{none}
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_con(mtcars, con = gzfile(tmp))
#' unmarshall_con(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall_con <- function(robj, con = NULL) {
  stopifnot(inherits(con, "connection"))
  if(!isOpen(con)){
    on.exit(close(con)) 
    open(con, "wb")
  }
  invisible(.Call(marshall_con_, robj, con))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Unserialize an R object from a raw vector 
#' 
#' @param raw_vec A raw vector containing serialized data 
#' 
#' @return Return the unserialized R object
#' @export
#'
#' @examples
#' m <- marshall_raw(head(mtcars))
#' m
#' unmarshall_raw(m)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall_raw <- function(raw_vec) {
  .Call(unmarshall_raw_, raw_vec)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Unserialize an R object from a connection
#' 
#' @param con A connection from which to read serialized data
#' 
#' @return Return the unserialized R object
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_con(mtcars, con = gzfile(tmp))
#' unmarshall_con(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall_con <- function(con) {
  if(!isOpen(con)){
    on.exit(close(con)) 
    open(con, "rb")
  }
  .Call(unmarshall_con_, con)
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

