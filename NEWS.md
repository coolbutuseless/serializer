# serializer 0.1.5   2021-04-11

* More orthogonality in the dynamic and static buffer techniques

# serializer 0.1.4   2020-09-24

* Rename: `calc_marshalled_size()` -> `calc_size_robust()`
* Rename: `marshall_minimize_malloc()` -> `marshall_fast()`
* Feature: `calc_size_fast()` a faster (but less robust) version of 
  `calc_size_robust()`

# serializer 0.1.3   2020-09-22

* Features: `marshall_minimize_malloc()` pre-calculates the resulting size of 
  the serialized representation. This minimizes memory allocations and 
  leads to faster serialization for medium to large objects

# serializer 0.1.2   2020-09-22

* Feature: `calc_marshalled_size()` added. This function will only calculate
  the size of the ersulting serialized object, but will not do any actual
  writing of bytes to the output.

# serializer 0.1.1   2020-09-22

* use 'marshall/unmarshall' function names instead of 'pack/unpack'
* Internal refactor of C code

# serializer 0.1.0   2020-09-17

* Initial release
