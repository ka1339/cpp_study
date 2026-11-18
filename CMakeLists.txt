cmake_minimum_required(VERSION 3.10.0)
project(CPP_TEST VERSION 0.1.0 LANGUAGES C CXX)

function(add_example name)
    add_executable(example_${name} ${ARGN})
    target_compile_features(example_${name} PRIVATE cxx_std_17)
endfunction()

add_example(namespaces examples/namespaces/main.cpp)
add_example(function_templates examples/function_templates/main.cpp)
add_example(class_templates examples/class_templates/main.cpp)
add_example(variadic_templates examples/variadic_templates/main.cpp)

