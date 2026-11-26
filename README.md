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

- `network_socket_receiver` and `network_socket_sender`: run the receiver and
  sender as separate programs to exchange a TCP echo message on loopback.
- `network_http_server` and `network_http_client`: run a local HTTP server and
  a separate client to inspect an HTTP/1.1 GET request and response.
- `network_mqtt`: build and print MQTT 3.1.1 CONNECT and QoS 0 PUBLISH packets
  in hexadecimal.
- `network_mqtt_broker`, `network_mqtt_subscriber`, and
  `network_mqtt_publisher`: run the broker first, then the subscriber, then
  the publisher. The demo broker listens only on `127.0.0.1:1883` by default;
  pass the same port as the first argument to all three programs to choose
  another port. The broker supports MQTT 3.1.1 clean sessions, QoS 0
  publish/subscribe, and topic filters with `+` and `#`. It is an educational
  example, not a production broker: it does not provide authentication,
  encryption, retained messages, persistence, or QoS 1/2.

On Windows, start each program in a separate terminal. For example:

```powershell
# Terminal 1
.\build\example_network_socket_receiver.exe
# Terminal 2
.\build\example_network_socket_sender.exe

# Terminal 1
.\build\example_network_http_server.exe
# Terminal 2
.\build\example_network_http_client.exe

# Terminal 1
.\build\example_network_mqtt_broker.exe
# Terminal 2
.\build\example_network_mqtt_subscriber.exe
# Terminal 3
.\build\example_network_mqtt_publisher.exe
```

Press `Ctrl+C` to stop the broker. The socket and HTTP receivers/servers
handle one connection per run; start the sender/client only after the
receiver/server is listening.

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
