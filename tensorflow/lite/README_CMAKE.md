# TensorFlow Lite tflite_cmake branch
This tflite_cmake branch creates an executable, model_test2 that is a c++ test for dumping per layer data in a useful format for debug.

The printf modifications aer enabled by the DUMP_PER_LAYER_DATA option, specified in the cmake file.

It requires that you specify the path to the tflite model, the path to an image, and the height of the image

It assumes height and with of the image are the same.

Output goes to stdout

The currently supported models are all quantized two mobilenets and four inception models.

All the models are fetched by the cmake operations.

This is currently tested only on ubuntu 18.04

The image format is expected to be uint8 RGB in the tflite quantized format.  

I'm using a tga format file in the tests, and ignoring the header info.

The cat test images are also installed in the dbg build sub-folders as part of the installation.

git clone https://github.com/jnorwood/tensorflow.git 

cd tensorflow

git checkout tflite_cmake

source tensorflow/lite/download_dependencies.sh

cd tensorflow/lite

mkdir dbg

cd dbg

cmake ..

make

make test

It is also possible to create an eclipse debug environment, for example with clang tools) by substituting the following commands after cd dbg, then import that dbg folder into eclipse as an existing project:

export CC=/usr/bin/clang

export CXX=/usr/bin/clang++

cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j8 ..

Below is an example of one of the tests, specifying the quantized model, an RGB uint8 format input file, the height of the input file in pixels and a redirect of output

./model_test2 "/home/jay/tensorflow/tensorflow/lite/dbg/mobilenetv1/mobilenet_v1_1.0_224_quant.tflite" "/home/jay/tensorflow/tensorflow/lite/dbg/_deps/images-src/tflite_quant/cat224ui8tf.npy" "224" >mod2.log




 



