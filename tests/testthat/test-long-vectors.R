
test_that("serialized length can be > max integer", {
  
  x <- raw(length = .Machine$integer.max)
  len <- calc_serialized_size(x)
  expect_true(len > 0)
  expect_true(len > .Machine$integer.max)
})
