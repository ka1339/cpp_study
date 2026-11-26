cmake_minimum_required(VERSION 3.12.0)
project(CPP_TEST VERSION 0.1.0 LANGUAGES C CXX)

file(WRITE "${CMAKE_BINARY_DIR}/cpp_targets.txt" "")

function(add_example name)
    add_executable(example_${name} ${ARGN})
    target_compile_features(example_${name} PRIVATE cxx_std_17)

    foreach(source IN LISTS ARGN)
        get_filename_component(
            source_path "${source}" ABSOLUTE
            BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        file(APPEND "${CMAKE_BINARY_DIR}/cpp_targets.txt" "${source_path}\texample_${name}\n")
    endforeach()
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
add_example(sync_mutexes examples/synchronization/mutexes/main.cpp)
add_example(sync_futures examples/synchronization/futures/main.cpp)
add_example(sync_semaphore examples/synchronization/semaphore/main.cpp)
add_example(sync_latch_barrier examples/synchronization/latch_barrier/main.cpp)
add_example(sync_jthread examples/synchronization/jthread/main.cpp)
add_example(
    thread_pool
    examples/thread_pool/main.cpp
    examples/thread_pool/src/thread_pool.cpp
)
target_include_directories(example_thread_pool PRIVATE examples/thread_pool/include)
add_example(network_socket_receiver examples/networking/socket/receiver.cpp)
add_example(network_socket_sender examples/networking/socket/sender.cpp)
add_example(network_http_server examples/networking/http/server.cpp)
add_example(network_http_client examples/networking/http/client.cpp)
add_example(network_mqtt examples/networking/mqtt/main.cpp)
add_example(network_mqtt_broker examples/networking/mqtt/broker.cpp)
add_example(network_mqtt_subscriber examples/networking/mqtt/subscriber.cpp)
add_example(network_mqtt_publisher examples/networking/mqtt/publisher.cpp)

set_target_properties(
    example_sync_atomic_counter
    example_sync_mutexes
    example_sync_futures
    example_sync_semaphore
    example_sync_latch_barrier
    example_sync_jthread
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

find_package(Threads REQUIRED)
target_link_libraries(example_sync_atomic_counter PRIVATE Threads::Threads)
target_link_libraries(example_sync_bounded_queue PRIVATE Threads::Threads)
target_link_libraries(example_sync_mutexes PRIVATE Threads::Threads)
target_link_libraries(example_sync_futures PRIVATE Threads::Threads)
target_link_libraries(example_sync_semaphore PRIVATE Threads::Threads)
target_link_libraries(example_sync_latch_barrier PRIVATE Threads::Threads)
target_link_libraries(example_sync_jthread PRIVATE Threads::Threads)
target_link_libraries(example_thread_pool PRIVATE Threads::Threads)
target_link_libraries(example_network_mqtt_broker PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(example_network_socket_receiver PRIVATE ws2_32)
    target_link_libraries(example_network_socket_sender PRIVATE ws2_32)
    target_link_libraries(example_network_http_server PRIVATE ws2_32)
    target_link_libraries(example_network_http_client PRIVATE ws2_32)
    target_link_libraries(example_network_mqtt_broker PRIVATE ws2_32)
    target_link_libraries(example_network_mqtt_subscriber PRIVATE ws2_32)
    target_link_libraries(example_network_mqtt_publisher PRIVATE ws2_32)
endif()

