cmake_minimum_required(VERSION 3.10.0)
project(CPP_TEST VERSION 0.1.0 LANGUAGES C CXX)

function(add_example name)
    add_executable(example_${name} ${ARGN})
    target_compile_features(example_${name} PRIVATE cxx_std_17)
endfunction()

add_example(
    namespaces
    examples/namespaces/main.cpp
    examples/namespaces/src/orders/order.cpp
    examples/namespaces/src/tax/tax.cpp
)
target_include_directories(example_namespaces PRIVATE examples/namespaces/include)
add_example(function_templates examples/function_templates/main.cpp)
add_example(class_templates examples/class_templates/main.cpp)
add_example(variadic_templates examples/variadic_templates/main.cpp)
add_example(class_initialization examples/class_initialization/main.cpp)
add_example(class_copying examples/class_copying/main.cpp)
add_example(operator_overloading examples/operator_overloading/main.cpp)
add_example(uniform_initialization examples/uniform_initialization/main.cpp)
add_example(standard_algorithms examples/standard_library/algorithms/main.cpp)
add_example(standard_optional examples/standard_library/optional/main.cpp)
add_example(standard_string_view examples/standard_library/string_view/main.cpp)
add_example(standard_variant examples/standard_library/variant/main.cpp)
add_example(data_vector examples/data_structures/vector/main.cpp)
add_example(data_deque examples/data_structures/deque/main.cpp)
add_example(data_set examples/data_structures/set/main.cpp)
add_example(data_map examples/data_structures/map/main.cpp)
add_example(data_unordered_map examples/data_structures/unordered_map/main.cpp)
add_example(data_stack examples/data_structures/stack/main.cpp)
add_example(data_queue examples/data_structures/queue/main.cpp)
add_example(sync_atomic_counter examples/synchronization/atomic_counter/main.cpp)
add_example(sync_bounded_queue examples/synchronization/bounded_queue/main.cpp)
add_example(
    thread_pool
    examples/thread_pool/main.cpp
    examples/thread_pool/src/thread_pool.cpp
)
target_include_directories(example_thread_pool PRIVATE examples/thread_pool/include)
find_package(Threads REQUIRED)
target_link_libraries(example_sync_atomic_counter PRIVATE Threads::Threads)
target_link_libraries(example_sync_bounded_queue PRIVATE Threads::Threads)
target_link_libraries(example_thread_pool PRIVATE Threads::Threads)

