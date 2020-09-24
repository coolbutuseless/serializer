

test_that("calc_size_fast and calc_size_robust agree", {

  set.seed(1)

  dat <- mtcars
  expect_true(calc_size_fast(dat) >= calc_size_robust(dat))

  dat <- iris
  expect_true(calc_size_fast(dat) >= calc_size_robust(dat))

  dat <- sample(1e6)
  expect_true(calc_size_fast(dat) >= calc_size_robust(dat))

  dat <- as.numeric(sample(1e5))
  expect_true(calc_size_fast(dat) >= calc_size_robust(dat))

  dat <- sample(letters, 500, replace = TRUE)
  expect_true(calc_size_fast(dat) >= calc_size_robust(dat))

})
