cmake_minimum_required(VERSION 3.13)
include(FetchContent)
include(ExternalProject)
option(ENABLE_TESTS "Enable tests" ON)
if (${ENABLE_TESTS})
    enable_testing()
endif()
set(FETCHCONTENT_QUIET OFF)

set(CMAKE_BUILD_TYPE Debug)

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

FetchContent_Declare(
  images
  GIT_REPOSITORY https://github.com/jnorwood/images.git
  GIT_TAG        origin/master
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

FetchContent_GetProperties(images)
if(NOT images_POPULATED)
  FetchContent_Populate(images)
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

set(TENSORFLOW_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(TENSORFLOW_LITE_ROOT_DIR "${TENSORFLOW_ROOT_DIR}/tensorflow/lite")
set(TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR "${TENSORFLOW_LITE_ROOT_DIR}/experimental/ruy")
set(TENSORFLOW_LITE_KERNEL_DIR "${TENSORFLOW_LITE_ROOT_DIR}/kernels")
set(TENSORFLOW_LITE_DOWNLOADED_DEPENDENCIES_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads")
set(TENSORFLOW_LITE_GEMMLOWP_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/gemmlowp")
set(TENSORFLOW_LITE_EIGEN_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/eigen")
set(TENSORFLOW_LITE_FLATBUFFERS_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/flatbuffers/include")
set(TENSORFLOW_LITE_FARMHASH_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/farmhash/src")
set(TENSORFLOW_LITE_FFT_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/fft2d")
set(TENSORFLOW_LITE_ABSL_DIR "${TENSORFLOW_LITE_ROOT_DIR}/downloads/absl")
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
	-DFLATBUFFERS_LOCALE_INDEPENDENT=0
    -DDUMP_PER_LAYER_DATA
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
    ${TENSORFLOW_LITE_FFT_DIR}	
    ${TENSORFLOW_LITE_ABSL_DIR}
   )
SET (TENSORFLOW_LITE_EXPERIMENTAL_RUY_SOURCES
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/allocator.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/blocking_counter.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/block_map.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/context.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/detect_dotprod.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/thread_pool.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/trmul.cc
    ${TENSORFLOW_LITE_EXPERIMENTAL_RUY_DIR}/tune.cc
)

SET (TENSORFLOW_LITE_KERNEL_SOURCES
    ${TENSORFLOW_LITE_KERNEL_DIR}/activations.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/audio_spectrogram.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/add.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/add_n.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/arg_min_max.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/audio_spectrogram.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/basic_rnn.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/batch_to_space_nd.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/bidirectional_sequence_rnn.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/bidirectional_sequence_lstm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/concatenation.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/cast.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/ceil.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/comparisons.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/concatenation.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/conv.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/cpu_backend_context.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/cpu_backend_gemm_eigen.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/cpu_backend_support.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/depthwise_conv.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/dequantize.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/detection_postprocess.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/div.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/eigen_support.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/embedding_lookup.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/embedding_lookup_sparse.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/elementwise.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/embedding_lookup.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/embedding_lookup_sparse.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/expand_dims.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/exp.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/fake_quant.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/fill.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/floor.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/floor_div.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/floor_mod.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/fully_connected.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/gather.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/gather_nd.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/gemmlowp_support.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/hashtable_lookup.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/if.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/kernel_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/optimized/neon_tensor_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/quantization_util.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/reference/portable_tensor_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/internal/tensor_utils.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/kernel_util.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/l2norm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/local_response_norm.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/logical.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/lsh_projection.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/lstm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/lstm_eval.cc
    #${TENSORFLOW_LITE_KERNEL_DIR}/mean.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/maximum_minimum.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/mfcc.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/internal/mfcc.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/internal/mfcc_dct.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/internal/mfcc_mel_filterbank.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/internal/quantization_util.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/internal/tensor_utils.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/internal/spectrogram.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/matrix_diag.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/matrix_set_diag.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/mirror_pad.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/mul.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/neg.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/one_hot.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/pack.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/pad.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/pooling.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/pow.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/comparisons.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/ceil.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/quantize.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/range.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/rank.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/reduce.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/register.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/register_ref.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/reshape.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/resize_bilinear.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/resize_nearest_neighbor.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/reverse.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/reverse_sequence.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/round.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/select.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/shape.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/skip_gram.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/slice.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/space_to_batch_nd.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/space_to_depth.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/sparse_to_dense.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/split.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/split_v.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/squared_difference.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/squeeze.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/strided_slice.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/sub.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/svdf.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/transpose.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/test_util.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/tile.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/topk_v2.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/transpose.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/transpose_conv.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/unique.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/unpack.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/where.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/while.cc
	${TENSORFLOW_LITE_KERNEL_DIR}/zeros_like.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/unidirectional_sequence_lstm.cc
    ${TENSORFLOW_LITE_KERNEL_DIR}/unidirectional_sequence_rnn.cc
    )

add_library(tensorflow-lite STATIC
    ${TENSORFLOW_LITE_ROOT_DIR}/simple_memory_arena.cc
    #${TENSORFLOW_LITE_ROOT_DIR}/context.c
	${TENSORFLOW_LITE_ROOT_DIR}/arena_planner.cc
	${TENSORFLOW_LITE_ROOT_DIR}/stderr_reporter.cc
	${TENSORFLOW_LITE_ROOT_DIR}/c/c_api_internal.c
    ${TENSORFLOW_LITE_ROOT_DIR}/core/api/error_reporter.cc
	${TENSORFLOW_LITE_ROOT_DIR}/core/api/op_resolver.cc
	${TENSORFLOW_LITE_ROOT_DIR}/core/api/flatbuffer_conversions.cc
	${TENSORFLOW_LITE_ROOT_DIR}/core/subgraph.cc
	${TENSORFLOW_LITE_ROOT_DIR}/graph_info.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/interpreter.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/string_util.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/allocation.cc
 	${TENSORFLOW_LITE_ROOT_DIR}/minimal_logging.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/model.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/mutable_op_resolver.cc
    #${TENSORFLOW_LITE_ROOT_DIR}/tflite_osal.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/delegates/nnapi/nnapi_delegate_disabled.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/nnapi/nnapi_implementation_disabled.cc
	${TENSORFLOW_LITE_ROOT_DIR}/mmap_allocation_disabled.cc
	${TENSORFLOW_LITE_ROOT_DIR}/minimal_logging_default.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/optional_debug_tools.cc
    ${TENSORFLOW_LITE_ROOT_DIR}/util.cc
    ${TENSORFLOW_LITE_KERNEL_SOURCES}
	${TENSORFLOW_LITE_EXPERIMENTAL_RUY_SOURCES}
    ${TENSORFLOW_LITE_FARMHASH_DIR}/farmhash.cc
	${TENSORFLOW_LITE_FFT_DIR}/fft4g.c  
	${TENSORFLOW_LITE_FFT_DIR}/fft4g_h.c  
	${TENSORFLOW_LITE_FFT_DIR}/fft8g.c  
	${TENSORFLOW_LITE_FFT_DIR}/fft8g_h.c  
	${TENSORFLOW_LITE_FFT_DIR}/fftsg.c  
	${TENSORFLOW_LITE_FFT_DIR}/fftsg_h.c	

    )



add_library(tensorflow-lite_test_util STATIC
    ${TENSORFLOW_LITE_KERNEL_DIR}/test_util.cc
    ${TENSORFLOW_ROOT_DIR}/tensorflow/core/platform/default/logging.cc
    ${TENSORFLOW_ROOT_DIR}/tensorflow/core/platform/env_time.cc
    ${PLATFORM_ENV_TIME_SRC}
  )

function(add_tflite_tests test_directory)
	file(GLOB TEST_SRC_FILES RELATIVE ${test_directory} ${test_directory}/model_test2.cc )
	foreach (TEST_SRC_FILE ${TEST_SRC_FILES})
      string(REPLACE ".cc" "" TEST_NAME ${TEST_SRC_FILE})
     add_executable(${TEST_NAME} ${test_directory}/${TEST_SRC_FILE} )
 	target_link_libraries(${TEST_NAME} tensorflow-lite gtest gmock tensorflow-lite_test_util dl)
     endforeach()

endfunction()


add_tflite_tests(${TENSORFLOW_LITE_ROOT_DIR})

add_test (NAME mobilenet_v1
	COMMAND model_test2 ${mobilenetv1_SOURCE_DIR}/mobilenet_v1_1.0_224_quant.tflite ${images_SOURCE_DIR}/tflite_quant/cat224ui8tf.npy 224
	OUTPUT_FILE mod2.txt
	)
add_test (NAME mobilenet_v2
	COMMAND model_test2 ${mobilenetv2_SOURCE_DIR}/mobilenet_v2_1.0_224_quant.tflite ${images_SOURCE_DIR}/tflite_quant/cat224ui8tf.npy 224
	OUTPUT_FILE mod2.txt
	)
add_test (NAME inception_v4
	COMMAND model_test2 ${inceptionv4_SOURCE_DIR}/inception_v4_299_quant.tflite ${images_SOURCE_DIR}/tflite_quant/cat_299.tga 299
	OUTPUT_FILE mod2.txt
	)
add_test (NAME inception_v3
	COMMAND model_test2 ${inceptionv3_SOURCE_DIR}/inception_v3_quant.tflite ${images_SOURCE_DIR}/tflite_quant/cat_299.tga 299
	OUTPUT_FILE mod2.txt
	)
add_test (NAME inception_v1
	COMMAND model_test2 ${inceptionv1_SOURCE_DIR}/inception_v1_224_quant.tflite ${images_SOURCE_DIR}/tflite_quant/cat224ui8tf.npy 224
	OUTPUT_FILE mod2.txt
	)
add_test (NAME inception_v2
	COMMAND model_test2 ${inceptionv2_SOURCE_DIR}/inception_v2_224_quant.tflite ${images_SOURCE_DIR}/tflite_quant/cat224ui8tf.npy 224
	OUTPUT_FILE mod2.txt
	)
