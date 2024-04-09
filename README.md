
<!-- README.md is generated from README.Rmd. Please edit that file -->

# serializer

<!-- badges: start -->

![](https://img.shields.io/badge/cool-useless-green.svg)
[![R-CMD-check](https://github.com/coolbutuseless/serializer/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/coolbutuseless/serializer/actions/workflows/R-CMD-check.yaml)
<!-- badges: end -->

`serializer` is a package which demonstrates how to use R’s internal
serialization interface from C. The code is the minimum amount of code
required to do this, and I’ve inserted plenty of comments for guidance.

This package was developed to help me figure out the serialization
process in R. It is perhaps only really interesting if you want to look
at and/or steal the C code. It’s under the [MIT
license](https://mit-license.org/), so please feel free to re-use in
your own projects.

## What’s in the box

- `marshall()`/`unmarshall()` are direct analogues for
  `base::serialize()` and `base::unserialize()`
  - These functions can serialize/unserialize from a raw vector or a
    connection
- `calc_serialized_size()` calculates the exact size of the serialized
  representation of an object using R’s seriazliation infrastructure but
  not actually allocating any bytes.

## Installation

You can install from
[GitHub](https://github.com/coolbutuseless/serializer) with:

``` r
# install.package('remotes')
remotes::install_github('coolbutuseless/serializer')
```

## Example - serialize/unserialize with raw vectors

``` r
library(serializer)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# The object to be serialized
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dat <- head(mtcars, 3)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Calculate exactly how many bytes this will take once serialized
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
serializer::calc_serialized_size(dat)
#> [1] 674

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Serialized results from this package and base::serialize should be identical
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v1 <- serializer::marshall(dat)
v2 <- base::serialize(dat, NULL, xdr = FALSE)
identical(v1, v2)
#> [1] TRUE

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# The serialized length should match the calculation from earlier
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
length(v1)
#> [1] 674
head(v1, 200)
#>   [1] 42 0a 03 00 00 00 03 03 04 00 00 05 03 00 05 00 00 00 55 54 46 2d 38 13 03
#>  [26] 00 00 0b 00 00 00 0e 00 00 00 03 00 00 00 00 00 00 00 00 00 35 40 00 00 00
#>  [51] 00 00 00 35 40 cd cc cc cc cc cc 36 40 0e 00 00 00 03 00 00 00 00 00 00 00
#>  [76] 00 00 18 40 00 00 00 00 00 00 18 40 00 00 00 00 00 00 10 40 0e 00 00 00 03
#> [101] 00 00 00 00 00 00 00 00 00 64 40 00 00 00 00 00 00 64 40 00 00 00 00 00 00
#> [126] 5b 40 0e 00 00 00 03 00 00 00 00 00 00 00 00 80 5b 40 00 00 00 00 00 80 5b
#> [151] 40 00 00 00 00 00 40 57 40 0e 00 00 00 03 00 00 00 33 33 33 33 33 33 0f 40
#> [176] 33 33 33 33 33 33 0f 40 cd cc cc cc cc cc 0e 40 0e 00 00 00 03 00 00 00 f6

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Unmarshall the raw bytes back into an object  
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
serializer::unmarshall(v1)
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710    22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
```

## Example - serialize/unserialize with connections

``` r
tmp <- tempfile()
serializer::marshall(head(mtcars, 3), gzfile(tmp))
serializer::unmarshall(gzfile(tmp))
#>                mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4     21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag 21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710    22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
```

## What’s the upper bound on serialization speed?

`calc_serialized_size()` can be used to calculate the size of a
serialized object, but does not actually try and create the serialized
object.

Because this does not do any memory allocation, or copying of bytes, the
speed of `calc_serialized_size()` should give an approximation of the
maximum throughput of the serialization process when using R’s internal
serialization mechanism.

The speeds below seem ridiculous, because at its core, serialization is
just passing *pointers* + *lengths* to an output stream, and doing very
very little actual memory allocation or copying.

``` r

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Test objects
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
N <- 1e7
obj1 <- data.frame(x = sample(N), y = runif(N))
obj2 <- do.call(rbind, replicate(1000, iris, simplify = FALSE))
obj3 <- sample(N)
obj4 <- sample(10)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Calc sizes of test objects
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(n1 <- lobstr::obj_size(obj1))
#> 120.00 MB
(n2 <- lobstr::obj_size(obj2))
#> 5.40 MB
(n3 <- lobstr::obj_size(obj3))
#> 40.00 MB
(n4 <- lobstr::obj_size(obj4))
#> 96 B

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# go through seritalization process, but only count the bytes
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
res <- bench::mark(
  calc_serialized_size(obj1),
  calc_serialized_size(obj2),
  calc_serialized_size(obj3),
  calc_serialized_size(obj4),
  check = FALSE
)


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# calc theoretical upper limit
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
res %>% 
  mutate(MB = round(as.numeric(c(n1, n2, n3, n4))/1024^2)) %>%
  mutate(`GB/s` = round(MB/1024 / as.numeric(median), 1)) %>%
  mutate(`itr/sec` = round(`itr/sec`)) %>%
  select(expression, median, `itr/sec`, MB, `GB/s`) %>%
  knitr::kable(caption = "Maximum possible throughput of serialization")
```

| expression                 |   median | itr/sec |  MB |    GB/s |
|:---------------------------|---------:|--------:|----:|--------:|
| calc_serialized_size(obj1) |   5.37µs |  173303 | 114 | 20727.4 |
| calc_serialized_size(obj2) |   1.68µs |  571156 |   5 |  2904.6 |
| calc_serialized_size(obj3) |   3.03µs |  319307 |  38 | 12231.1 |
| calc_serialized_size(obj4) | 984.06ns | 1052272 |   0 |     0.0 |

Maximum possible throughput of serialization
