% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/serialize.R
\name{calc_size_robust}
\alias{calc_size_robust}
\title{Calculate the size of the R object when it is serialized}
\usage{
calc_size_robust(robj)
}
\arguments{
\item{robj}{R object}
}
\description{
This function goes through all the motions of serializing an object, but
does nothing with the bytes other than to tally the total length.
}
