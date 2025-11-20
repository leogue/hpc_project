cmake_minimum_required(VERSION 3.10)
project(hpc_project C)

set(CMAKE_C_STANDARD 99)


if(APPLE)
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.dylib")
endif()

find_package(OpenMP REQUIRED)


add_executable(matrix_vector_seq
        ex2/matrix-vector/matrix_vector_seq.c
        utils/utils.c
)

add_executable(matrix_vector_omp
        ex2/matrix-vector/matrix_vector_omp.c
        utils/utils.c
)

add_executable(matrix_vector_mpi
        ex2/matrix-vector/matrix_vector_mpi.c
        utils/utils.c
)

add_executable(matrix_power_seq
        ex2/matrix-power/matrix_power_seq.c
        utils/utils.c
)

add_executable(matrix_power_omp
        ex2/matrix-power/matrix_power_omp.c
        utils/utils.c
)


target_link_libraries(matrix_vector_seq PRIVATE OpenMP::OpenMP_C)
target_link_libraries(matrix_vector_omp PRIVATE OpenMP::OpenMP_C)
target_link_libraries(matrix_power_seq PRIVATE OpenMP::OpenMP_C)
target_link_libraries(matrix_power_omp PRIVATE OpenMP::OpenMP_C)