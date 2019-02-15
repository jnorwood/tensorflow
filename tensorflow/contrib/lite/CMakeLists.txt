cmake_minimum_required(VERSION 3.13)
include(FetchContent)
include(ExternalProject)
FetchContent_Declare(
  mobilenetv1
  URL http://download.tensorflow.org/models/mobilenet_v1_2018_08_02/mobilenet_v1_1.0_224_quant.tgz
  URL_HASH MD5=36af340c00e60291931cb30ce32d4e86
  SOURCE_DIR  mobilenetv1
)

FetchContent_Declare(
  mobilenetv2
  URL http://download.tensorflow.org/models/tflite_11_05_08/mobilenet_v2_1.0_224_quant.tgz
  URL_HASH MD5=04b4cc2536e1a51b9d29e98921b4970b
  SOURCE_DIR  mobilenetv2
)
FetchContent_Declare(
  inceptionv1
  URL http://download.tensorflow.org/models/inception_v1_224_quant_20181026.tgz
  #URL_HASH MD5=04b4cc2536e1a51b9d29e98921b4970b
  SOURCE_DIR  inceptionv1
)
FetchContent_Declare(
  inceptionv2
  URL http://download.tensorflow.org/models/inception_v2_224_quant_20181026.tgz
  #URL_HASH MD5=04b4cc2536e1a51b9d29e98921b4970b
  SOURCE_DIR  inceptionv2
)
FetchContent_Declare(
  inceptionv3
  URL http://download.tensorflow.org/models/tflite_11_05_08/inception_v3_quant.tgz
  #URL_HASH MD5=04b4cc2536e1a51b9d29e98921b4970b
  SOURCE_DIR  inceptionv3
)
FetchContent_Declare(
  inceptionv4
  URL http://download.tensorflow.org/models/inception_v4_299_quant_20181026.tgz
  #URL_HASH MD5=04b4cc2536e1a51b9d29e98921b4970b
  SOURCE_DIR  inceptionv4
)


FetchContent_GetProperties(mobilenetv1)
if(NOT mobilenetv1_POPULATED)
  FetchContent_Populate(mobilenetv1)
endif()

FetchContent_GetProperties(mobilenetv2)
if(NOT mobilenetv2_POPULATED)
  FetchContent_Populate(mobilenetv2)
endif()

FetchContent_GetProperties(inceptionv1)
if(NOT inceptionv1_POPULATED)
  FetchContent_Populate(inceptionv1)
endif()
FetchContent_GetProperties(inceptionv2)
if(NOT inceptionv2_POPULATED)
  FetchContent_Populate(inceptionv2)
endif()
FetchContent_GetProperties(inceptionv3)
if(NOT inceptionv3_POPULATED)
  FetchContent_Populate(inceptionv3)
endif()
FetchContent_GetProperties(inceptionv4)
if(NOT inceptionv4_POPULATED)
  FetchContent_Populate(inceptionv4)
endif()


# Project
project(tensorflow-lite C CXX)


# From https://stackoverflow.com/questions/10113017/setting-the-msvc-runtime-in-cmake
macro(configure_msvc_runtime)
  if(MSVC)
    # Default to statically-linked runtime.
    if("${MSVC_RUNTIME}" STREQUAL "")
      set(MSVC_RUNTIME "static")
    endif()
    # Set compiler options.
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    if(${MSVC_RUNTIME} STREQUAL "static")
      message(STATUS
        "MSVC -> forcing use of statically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
        endif()
      endforeach()
    else()
      message(STATUS
        "MSVC -> forcing use of dynamically-linked runtime."
      )
      foreach(variable ${variables})
        if(${variable} MATCHES "/MT")
          string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
        endif()
      endforeach()
    endif()
  endif()
endmacro()






# Set C++14 as standard for the whole project
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set(SDK_DIR googletest)
set(GTEST_LIB_DIR downloads/googletest/googlemock/gtest)
set(GMOCK_LIB_DIR downloads/googletest/googlemock)

set(TENSORFLOW_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set(TENSORFLOW_LITE_ROOT_DIR "${TENSORFLOW_ROOT_DIR}/tensorflow/contrib/lite")
set(TENSORFLOW_LITE_KERNEL_DIR "${TENSORFLOW_LITE_ROOT_DIR}/kernels")
set(TENSORFLOW_LITE_DOWNLOADED_DEPENDENCIES_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads")
set(TENSORFLOW_LITE_GEMMLOWP_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/gemmlowp")
set(TENSORFLOW_LITE_EIGEN_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/eigen")
set(TENSORFLOW_LITE_FLATBUFFERS_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/flatbuffers/include")
set(TENSORFLOW_LITE_FARMHASH_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/farmhash/src")
set(TENSORFLOW_LITE_GMOCK_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/googletest/googlemock/include")
set(TENSORFLOW_LITE_GTEST_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/googletest/googletest/include")

add_subdirectory( ${TENSORFLOW_LITE_ROOT_DIR}/downloads/googletest/googlemock)
#add_subdirectory( ${TENSORFLOW_LITE_ROOT_DIR}/downloads/googletest/googletest )


if (WIN32)
    set(PLATFORM_ENV_TIME_SRC ${TENSORFLOW_ROOT_DIR}/tensorflow/core/platform/windows/env_time.cc)
    configure_msvc_runtime()
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
else()
    set(PLATFORM_ENV_TIME_SRC ${TENSORFLOW_ROOT_DIR}/tensorflow/core/platform/env_time.cc)
    set(variables
      CMAKE_C_FLAGS_DEBUG
      CMAKE_C_FLAGS_MINSIZEREL
      CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_RELWITHDEBINFO
      CMAKE_CXX_FLAGS_DEBUG
      CMAKE_CXX_FLAGS_MINSIZEREL
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )

endif (WIN32)


# TODO: SIMD Visual studio
add_definitions(
    -DGEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK
    -DNOMINMAX
    -DFARMHASH_NO_BUILTIN_EXPECT)


include_directories(${TENSORFLOW_ROOT_DIR}
    ${TENSORFLOW_LITE_GEMMLOWP_DIR}
    ${TENSORFLOW_LITE_FLATBUFFERS_DIR}
    ${TENSORFLOW_LITE_EIGEN_DIR}
    ${TENSORFLOW_LITE_FARMHASH_DIR}
    ${TENSORFLOW_LITE_GMOCK_DIR}
    ${TENSORFLOW_LITE_GTEST_DIR}
    )

SET (TENSORFLOW_LITE_KERNEL_SOURCES
    ${TENSORFLOW_LITE_KERNEL_DIR}/activations.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/add.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/basic_rnn.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/batch_to_space_nd.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/concatenation.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/conv.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/depthwise_conv.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/embedding_lookup.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/embedding_lookup_sparse.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/fully_connected.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/gather.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/gemm_support.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/hashtable_lookup.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/optimized/neon_tensor_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/quantization_util.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/reference/portable_tensor_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/tensor_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/kernel_util.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/l2norm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/local_response_norm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/lsh_projection.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/lstm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/mean.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/mul.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/pad.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/pooling.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/register.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/reshape.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/resize_bilinear.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/skip_gram.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/space_to_batch_nd.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/space_to_depth.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/svdf.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/transpose.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/unidirectional_sequence_rnn.cc
    )

add_library(tensorflow-lite STATIC
    ${TENSORFLOW_LITE_ROOT_DIR}/simple_memory_arena.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/context.c
    ${TENSORFLOW_LITE_ROOT_DIR}/error_reporter.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/interpreter.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/string_util.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/allocation.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/model.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/tflite_osal.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/nnapi_delegate.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/optional_debug_tools.cc
    ${TENSORFLOW_LITE_KERNEL_SOURCES}
    ${TENSORFLOW_LITE_FARMHASH_DIR}/farmhash.cc
    )



add_library(tensorflow-lite_test_util STATIC
    ${TENSORFLOW_LITE_KERNEL_DIR}/test_util.cc
    ${TENSORFLOW_ROOT_DIR}/tensorflow/core/platform/default/logging.cc
    ${TENSORFLOW_ROOT_DIR}/tensorflow/core/platform/env_time.cc
    ${PLATFORM_ENV_TIME_SRC}
  )

function(add_tflite_tests test_directory)
	file(GLOB TEST_SRC_FILES RELATIVE ${test_directory} ${test_directory}/model_test.cc )
	foreach (TEST_SRC_FILE ${TEST_SRC_FILES})
      string(REPLACE ".cc" "" TEST_NAME ${TEST_SRC_FILE})
     add_executable(${TEST_NAME} ${test_directory}/${TEST_SRC_FILE} )
 	target_link_libraries(${TEST_NAME} tensorflow-lite gtest gmock tensorflow-lite_test_util dl)
     endforeach()

endfunction()


add_tflite_tests(${TENSORFLOW_LITE_ROOT_DIR})

