
<!-- README.md is generated from README.Rmd. Please edit that file -->

# serializer

<!-- badges: start -->

![](https://img.shields.io/badge/cool-useless-green.svg) [![Lifecycle:
experimental](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://www.tidyverse.org/lifecycle/#experimental)
[![R build
status](https://github.com/coolbutuseless/serializer/workflows/R-CMD-check/badge.svg)](https://github.com/coolbutuseless/serializer/actions)
<!-- badges: end -->

`serializer` is a package which demonstrates how to use R’s internal
serialization interface from C. The code is the minimum amount of code
required to do this, and I’ve inserted plenty of comments for guidance.

This package was developed to help me figure out the serialization
process in R. It is perhaps only really interesting if you want to look
at and/or steal the C code. It’s under the [MIT
license](https://mit-license.org/), so please feel free to re-use in
your own projects.

If you want a rock solid version of this package that already exists,
use
[RApiSerialize](https://cran.r-project.org/web/packages/RApiSerialize/index.html).

## Installation

You can install from
[GitHub](https://github.com/coolbutuseless/serializer) with:

``` r
# install.package('remotes')
remotes::install_github('coolbutuseless/serializer')
```

## Notes

  - Using R’s serialization infrastructure from C involves 2 main parts:
      - a buffer (which could be memory, a file, a pipe, etc) with
        accompanying functions for reading and writing bytes to/from the
        buffer
      - input/output stream wrappers around this buffer initialised and
        created using R internals
          - Input stream: `R_inpstream_st`, `R_InitInPStream()`
          - Output stream: `R_outpstream_st`, `R_InitOutPStream()`

## Example

``` r
library(serializer)

v1 <- serializer::marshall(head(mtcars))
v2 <- base::serialize(mtcars, NULL, xdr = FALSE)

identical(v1, v2)
#> [1] FALSE

head(v1, 200)
#>   [1] 42 0a 03 00 00 00 02 00 04 00 00 05 03 00 05 00 00 00 55 54 46 2d 38 13 03
#>  [26] 00 00 0b 00 00 00 0e 00 00 00 06 00 00 00 00 00 00 00 00 00 35 40 00 00 00
#>  [51] 00 00 00 35 40 cd cc cc cc cc cc 36 40 66 66 66 66 66 66 35 40 33 33 33 33
#>  [76] 33 b3 32 40 9a 99 99 99 99 19 32 40 0e 00 00 00 06 00 00 00 00 00 00 00 00
#> [101] 00 18 40 00 00 00 00 00 00 18 40 00 00 00 00 00 00 10 40 00 00 00 00 00 00
#> [126] 18 40 00 00 00 00 00 00 20 40 00 00 00 00 00 00 18 40 0e 00 00 00 06 00 00
#> [151] 00 00 00 00 00 00 00 64 40 00 00 00 00 00 00 64 40 00 00 00 00 00 00 5b 40
#> [176] 00 00 00 00 00 20 70 40 00 00 00 00 00 80 76 40 00 00 00 00 00 20 6c 40 0e

serializer::unmarshall(v1)
#>                    mpg cyl disp  hp drat    wt  qsec vs am gear carb
#> Mazda RX4         21.0   6  160 110 3.90 2.620 16.46  0  1    4    4
#> Mazda RX4 Wag     21.0   6  160 110 3.90 2.875 17.02  0  1    4    4
#> Datsun 710        22.8   4  108  93 3.85 2.320 18.61  1  1    4    1
#> Hornet 4 Drive    21.4   6  258 110 3.08 3.215 19.44  1  0    3    1
#> Hornet Sportabout 18.7   8  360 175 3.15 3.440 17.02  0  0    3    2
#> Valiant           18.1   6  225 105 2.76 3.460 20.22  1  0    3    1
```

## What’s the upper bound on serialization speed?

`calc_marshalled_size()` can be used to calculate the size of a
serialized object, but does not actually try and create the serialized
object.

Because this does not do any memory allocation, or copying of bytes, the
speed of `calc_marshalled_size()` should give an approximation of the
maximum throughput of the serialization process when using R’s internal
serialization mechanism.

The speeds below seem ridiculous, because at its core, serialization is
just passing *pointers* + *lengths* to an output stream, and doing very
very little actual memory allocation or copying.

``` r
N <- 1e7
obj1 <- data.frame(
  x = sample(N),
  y = runif(N)
)

obj2 <- do.call(rbind, replicate(1000, iris, simplify = FALSE))

obj3 <- sample(N)

obj4 <- sample(10)

(n1 <- lobstr::obj_size(obj1))
#> 120,000,848 B
(n2 <- lobstr::obj_size(obj2))
#> 5,401,800 B
(n3 <- lobstr::obj_size(obj3))
#> 40,000,048 B
(n4 <- lobstr::obj_size(obj4))
#> 96 B

res <- bench::mark(
  calc_marshalled_size(obj1),
  calc_marshalled_size(obj2),
  calc_marshalled_size(obj3),
  calc_marshalled_size(obj4),
  check = FALSE
)


res %>% 
  mutate(MB = round(c(n1, n2, n3, n4)/1024)) %>%
  mutate(`GB/s` = round(MB/1024^2 / as.numeric(median), 1)) %>%
  mutate(`itr/sec` = round(`itr/sec`)) %>%
  select(expression, median, `itr/sec`, MB, `GB/s`) %>%
  knitr::kable(caption = "Maximum possible throughput of serialization")
```

| expression                   |  median | itr/sec |     MB |   GB/s |
| :--------------------------- | ------: | ------: | -----: | -----: |
| calc\_marshalled\_size(obj1) | 11.55µs |   81297 | 117188 | 9676.1 |
| calc\_marshalled\_size(obj2) |  5.75µs |  154474 |   5275 |  874.4 |
| calc\_marshalled\_size(obj3) |  5.59µs |  156696 |  39063 | 6658.3 |
| calc\_marshalled\_size(obj4) |  2.59µs |  294692 |      0 |    0.0 |

Maximum possible throughput of serialization

## Related Software

  - [RApiSerialize](https://cran.r-project.org/web/packages/RApiSerialize/index.html)
  - [qs](https://cran.r-project.org/web/packages/qs/index.html)
  - [fst](https://cran.r-project.org/web/packages/fst/index.html)

## Acknowledgements

  - R Core for developing and maintaining the language.
  - CRAN maintainers, for patiently shepherding packages onto CRAN and
    maintaining the repository
