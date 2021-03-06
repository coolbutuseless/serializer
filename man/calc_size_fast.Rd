% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/serialize.R
\name{calc_size_fast}
\alias{calc_size_fast}
\title{Calculate the approximate size of the R object when it is serialized}
\usage{
calc_size_fast(robj)
}
\arguments{
\item{robj}{R object}
}
\description{
This function returns a size which is greater-than-or-equal-to that returned by
\code{calc_size_robust}.
}
\details{
This function is faster than \code{calc_size_robust()} but has little support
for language and function objects.  If unsupported language objects are found
then this function will return a negative value.
}
