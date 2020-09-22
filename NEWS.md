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
