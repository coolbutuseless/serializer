


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
#' Serialize an R object to a file
#'
#' @param robj R object
#' @param filename filename
#'
#' @return None
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_file(head(mtcars), tmp)
#' unmarshall_file(tmp)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall_file <- function(robj, filename) {
  invisible(.Call(marshall_file_, robj, normalizePath(filename, mustWork = FALSE)))
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
#' Serialize an R object to a connection using an non-API function from R
#'
#' This approach will not pass \code{R CMD check} as the function `R_WriteConnection()`
#' is not part of the R API, but it is much much faster than the officially
#' blessed method of calling-back-into-R to write to a connection.
#' 
#' @inheritParams marshall_raw
#' @param con connection to write the data to
#'
#' @return \code{none}
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_con_illegal(mtcars, con = gzfile(tmp))
#' unmarshall_con_illegal(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall_con_illegal <- function(robj, con = NULL) {
  stopifnot(inherits(con, "connection"))
  if(!isOpen(con)){
    on.exit(close(con)) 
    open(con, "wb")
  }
  invisible(.Call(marshall_con_illegal_, robj, con))
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
#' Unserialize an R object from a file
#' 
#' @param filename a file containing serialized data 
#' 
#' @return Return the unserialized R object
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_file(head(mtcars), tmp)
#' unmarshall_file(tmp)
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall_file <- function(filename) {
  .Call(unmarshall_file_, normalizePath(filename, mustWork = TRUE))
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
#' Unserialize an R object from a connection using an non-API function from R
#'
#' This approach will not pass \code{R CMD check} as the function `R_ReadConnection()`
#' is not part of the R API, but it is much much faster than the officially
#' blessed method of calling-back-into-R to read from a connection.
#' 
#' @param con A connection from which to read serialized data
#' 
#' @return Return the unserialized R object
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_con_illegal(mtcars, con = gzfile(tmp))
#' unmarshall_con_illegal(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall_con_illegal <- function(con) {
  if(!isOpen(con)){
    on.exit(close(con)) 
    open(con, "rb")
  }
  .Call(unmarshall_con_illegal_, con)
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Serialize an R object to a connection using an non-API function from R
#'
#' This approach will not pass \code{R CMD check} as the function `R_WriteConnection()`
#' is not part of the R API, but it is much much faster than the officially
#' blessed method of calling-back-into-R to write to a connection.
#' 
#' @inheritParams marshall_raw
#' @param con connection to write the data to
#'
#' @return \code{none}
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_con_smuggle(mtcars, con = gzfile(tmp))
#' unmarshall_con_smuggle(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
marshall_con_smuggle <- function(robj, con = NULL) {
  stopifnot(inherits(con, "connection"))
  if(!isOpen(con)){
    on.exit(close(con)) 
    open(con, "wb")
  }
  invisible(.Call(marshall_con_smuggle_, robj, con))
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Unserialize an R object from a connection using an non-API function from R
#'
#' This approach will not pass \code{R CMD check} as the function `R_ReadConnection()`
#' is not part of the R API, but it is much much faster than the officially
#' blessed method of calling-back-into-R to read from a connection.
#' 
#' @param con A connection from which to read serialized data
#' 
#' @return Return the unserialized R object
#' @export
#'
#' @examples
#' tmp <- tempfile()
#' marshall_con_smuggle(mtcars, con = gzfile(tmp))
#' unmarshall_con_smuggle(gzfile(tmp))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unmarshall_con_smuggle <- function(con) {
  if(!isOpen(con)){
    on.exit(close(con)) 
    open(con, "rb")
  }
  .Call(unmarshall_con_smuggle_, con)
}



#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#' Init smuggle
#' 
#' @param rc_xptr read connection
#' 
#' @return None
#' @export
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
init_smuggle <- function(rc_xptr) {
  .Call(init_smuggle_, rc_xptr)
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

