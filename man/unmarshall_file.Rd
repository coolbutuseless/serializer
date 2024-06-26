% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/serialize.R
\name{unmarshall_file}
\alias{unmarshall_file}
\title{Unserialize an R object from a file}
\usage{
unmarshall_file(filename)
}
\arguments{
\item{filename}{a file containing serialized data}
}
\value{
Return the unserialized R object
}
\description{
Unserialize an R object from a file
}
\examples{
tmp <- tempfile()
marshall_file(head(mtcars), tmp)
unmarshall_file(tmp)
}
