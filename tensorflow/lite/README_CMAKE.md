# TensorFlow Lite tflite_cmake branch
This tflite_cmake branch creates an executable, model_test2 that is a c++ test for dumping per layer data in a useful format for debug.

It requires that you specify the path to the tflite model, the path to an image, and the height of the image

It assumes height and with of the image are the same.

Output goes to stdout

The currently supported models are all quantized two mobilenets and four inception models.

All the models are fetched by the cmake operations.

This is currently tested only on ubuntu 18.04

The image format is expected to be uint8 RGB in the tflite quantized format.  

I'm using a tga format file in the tests, and ignoring the header info.

The cat test images are also installed in the dbg build sub-folders as part of the installation.

https://github.com/jnorwood/tensorflow.git 

cd tensorflow

git checkout tflite_cmake

source tensorflow/lite/download_dependencies.sh

cd tensorflow/lite

mkdir dbg

cd dbg

cmake ..

make

make test
 



