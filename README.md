# C++ Examples

Each example lives in its own directory under `examples/` and builds as a
separate executable named `example_<name>`. Examples use C++17 unless noted;
the modern synchronization examples use C++20.

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

## Standard library examples

- `standard_algorithms`: use standard algorithms such as `sort` and
  `find_if` when a standard operation expresses the task clearly; prefer them
  over hand-written loops for common transformations and searches.
- `standard_optional`: use `optional` when a function may have no result and
  that absence is a normal outcome. Use an error/result type when callers need
  details about why an operation failed.
- `standard_string_view`: use `string_view` for read-only text parameters when
  the callee does not need ownership. Do not retain it past the lifetime of
  the referenced string; use `string` when the callee must own the text.
- `standard_variant`: use `variant` when a value can be one of a closed set of
  known types. Use a class hierarchy or another abstraction when alternatives
  are open-ended or need shared runtime behavior.

## Data structure examples

- `data_vector`: choose `vector` for contiguous storage, fast indexed access,
  and efficient append at the end; middle insertions and erasures move later
  elements.
- `data_deque`: choose `deque` for efficient insertion/removal at both ends
  with indexed access, when contiguous storage is not required.
- `data_set`: choose `set` for sorted, unique keys and logarithmic lookup;
  iteration follows key order and duplicate values are ignored.
- `data_map`: choose `map` when keys must stay sorted or ordered traversal and
  predictable logarithmic lookup are needed.
- `data_unordered_map`: choose `unordered_map` for average constant-time key
  lookup when iteration order is irrelevant; hash collisions can degrade
  performance.
- `data_stack`: choose `stack` when only the most recently added item should
  be accessed or removed (LIFO), such as undo history.
- `data_queue`: choose `queue` when items should be processed in arrival order
  (FIFO), such as pending requests.

## Concurrency examples

- `sync_atomic_counter`: use an atomic for independent, simple shared values
  that need indivisible updates, and `wait`/`notify` to block until an atomic
  value changes. Use a mutex when an invariant spans multiple values or an
  operation cannot be expressed atomically.
- `sync_bounded_queue`: use a mutex to protect related queue state and
  condition variables when threads should sleep until a predicate becomes
  true. Do not use a condition variable as a replacement for the mutex that
  protects the predicate and shared state.
- `sync_mutexes`: protect exclusive updates with `mutex` and `lock_guard`,
  safely lock multiple mutexes with `scoped_lock`, and allow concurrent reads
  with `shared_mutex` and `shared_lock`; use `call_once` for one-time setup.
- `sync_futures`: return values from asynchronous work with `async`/`future`
  and communicate a result from a thread with `promise`/`future`.
- `sync_semaphore`: use a counting semaphore to limit how many threads can
  access a fixed number of resources at once.
- `sync_latch_barrier`: use a latch for one-time coordination and a barrier to
  synchronize participating threads at repeated phase boundaries.
- `sync_jthread`: use `jthread` for automatic joining and `stop_token` with a
  stop-aware condition-variable wait for cooperative cancellation.
- `thread_pool`: register named commands, queue requests for worker threads,
  then drain pending work and join workers after the `exit` command is queued.
  The module is in `examples/thread_pool/include/` and `src/`.

## Networking examples

- `network_socket`: open a TCP listener on the loopback interface and exchange
  an echo message, showing socket creation, connect/accept, send/receive, and
  shutdown without contacting another machine.
- `network_http`: send a basic HTTP/1.1 GET request to a local server and
  inspect the request and response headers and body.
- `network_mqtt`: build and print MQTT 3.1.1 CONNECT and QoS 0 PUBLISH packets
  in hexadecimal. This packet-format example does not require an MQTT broker.

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

## Build and run the active file in VS Code

Open a `.cpp` file that belongs to a CMake executable target, then press
`Ctrl+Shift+B` and choose **Build and run active C++ file** if prompted. The
task configures the CMake project, builds the target containing the active
file, and runs its executable in the terminal. Interactive examples can read
input from that terminal.
