% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/serialize.R
\name{marshall_file}
\alias{marshall_file}
\title{Serialize an R object to a file}
\usage{
marshall_file(robj, filename)
}
\arguments{
\item{robj}{R object}

\item{filename}{filename}
}
\value{
None
}
\description{
Serialize an R object to a file
}
\examples{
tmp <- tempfile()
marshall_file(head(mtcars), tmp)
unmarshall_file(tmp)
}
