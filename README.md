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

## Namespace examples

- `namespaces`: organize a multi-component project into team-owned namespaces
  and public headers, qualify APIs to avoid collisions, and keep implementation
  helpers private to their source files.
- Prefer a unique organization or product namespace, then use subnamespaces for
  components. Keep public headers focused on API declarations and required
  dependencies; avoid `using namespace` in headers and qualify cross-component
  names. Put source-only helpers in an unnamed namespace in a `.cpp` file.
  Namespaces organize names but do not provide access control.

## Class initialization examples

- `class_initialization`: initialize every member, respect member declaration
  order, delegate constructors, validate invariants, and call virtual behavior
  only after the derived object is fully constructed.
- `class_copying`: compare shared-state shallow copies with independent deep
  copies, including copy constructors and copy assignment operators.
- `uniform_initialization`: see why braces help avoid uninitialized values and
  narrowing conversions, and when parentheses are needed to select the intended
  constructor instead of an `initializer_list` overload.

## Operator overloading examples

- `operator_overloading`: implement compound assignment, addition, equality,
  and stream output while preserving familiar operator behavior.

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
