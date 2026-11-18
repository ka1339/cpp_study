# C++ Examples

Each example lives in its own directory under `examples/` and builds as a
separate executable named `example_<name>`. Examples are compiled as C++17.

## Template examples

- `function_templates`: use an iterator and projection to sum values from a
  collection.
- `class_templates`: implement a reusable in-memory repository for different
  key and value types.
- `variadic_templates`: build a type-safe log message from any number of
  streamable values.

## Add an example

1. Create a directory such as `examples/vectors/`.
2. Add `main.cpp` there (and any other source or header files for the example).
3. Register its source files in the root `CMakeLists.txt`:

   ```cmake
   add_example(vectors examples/vectors/main.cpp)
   ```

4. Configure and build:

   ```powershell
   cmake -S . -B build
   cmake --build build
   ```

The resulting executable is `build/example_vectors.exe` on Windows.
