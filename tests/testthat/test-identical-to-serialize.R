


test_that("identical to base::serialize()", {


  expect_identical(
    serialize(mtcars, NULL, xdr = FALSE),
    marshall_raw(mtcars)
  )

  yy <- serialize(mtcars, NULL, xdr = FALSE)

  expect_identical(
    unserialize(yy),
    unmarshall_raw(yy)
  )




  set.seed(1)
  for (i in 1:10) {
    zz <- runif(1000)

    expect_identical(
      serialize(zz, NULL, xdr = FALSE),
      marshall_raw(zz)
    )

    yy <- serialize(zz, NULL, xdr = FALSE)

    expect_identical(
      unserialize(yy),
      unmarshall_raw(yy)
    )


  }








})
