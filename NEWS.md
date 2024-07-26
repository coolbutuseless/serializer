
# serializer 0.2.2   2024-07-27

* Support serialized sizes greater than max int

# serializer 0.2.1   2024-04-10

* Switch to SI size units in benchmarking.
* Documentation refresh
* Split `marshall()`/`unmarshall()` into
    * `marshall_raw()`/`unmarshall_raw()` for raw vectors
    * `marshall_con()`/`unmarshall_con()` for connections
* Small refactor of buffer handling

# serializer 0.2.0   2024-04-09

* Remove the dynamic buffer example. All work is now performed with static buffers.
* Added support for serializing with connection objects

# serializer 0.1.6   2021-12-11

* Remove `calc_size_fast()` as it's just a little too hacky.
* Rename `calc_serialized_size()` to `calc_size()`

# serializer 0.1.5   2021-04-11

* More orthogonality in the dynamic and static buffer techniques

# serializer 0.1.4   2020-09-24

* Rename: `calc_marshalled_size()` -> `calc_serialized_size()`
* Rename: `marshall_minimize_malloc()` -> `marshall_fast()`
* Feature: `calc_size_fast()` a faster (but less robust) version of 
  `calc_serialized_size()`

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
