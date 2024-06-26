% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/serialize.R
\name{unmarshall_raw}
\alias{unmarshall_raw}
\title{Unserialize an R object from a raw vector}
\usage{
unmarshall_raw(raw_vec)
}
\arguments{
\item{raw_vec}{A raw vector containing serialized data}
}
\value{
Return the unserialized R object
}
\description{
Unserialize an R object from a raw vector
}
\examples{
m <- marshall_raw(head(mtcars))
m
unmarshall_raw(m)
}
