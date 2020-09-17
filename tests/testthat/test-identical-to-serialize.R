


test_that("identical to base::serialize()", {


  expect_identical(
    serialize(mtcars, NULL, xdr = FALSE),
    pack(mtcars)
  )

  yy <- serialize(mtcars, NULL, xdr = FALSE)

  expect_identical(
    unserialize(yy),
    unpack(yy)
  )




  set.seed(1)
  for (i in 1:100) {
    zz <- runif(1000)

    expect_identical(
      serialize(zz, NULL, xdr = FALSE),
      pack(zz)
    )

    yy <- serialize(zz, NULL, xdr = FALSE)

    expect_identical(
      unserialize(yy),
      unpack(yy)
    )


  }








})
