/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>

#include "tensorflow/lite/model.h"

#include <gtest/gtest.h>
#include "tensorflow/lite/core/api/error_reporter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/testing/util.h"

// Comparison for TfLiteRegistration. Since TfLiteRegistration is a C object,
// we must declare this in global namespace, so argument-dependent operator
// lookup works.
inline bool operator==(const TfLiteRegistration& a,
                       const TfLiteRegistration& b) {
  return a.invoke == b.invoke && a.init == b.init && a.prepare == b.prepare &&
         a.free == b.free;
}

namespace tflite {

// Provide a dummy operation that does nothing.
namespace {
void* dummy_init(TfLiteContext*, const char*, size_t) { return nullptr; }
void dummy_free(TfLiteContext*, void*) {}
TfLiteStatus dummy_resize(TfLiteContext*, TfLiteNode*) { return kTfLiteOk; }
TfLiteStatus dummy_invoke(TfLiteContext*, TfLiteNode*) { return kTfLiteOk; }
TfLiteRegistration dummy_reg = {dummy_init, dummy_free, dummy_resize,
                                dummy_invoke};
}  // namespace

// Provide a trivial resolver that returns a constant value no matter what
// op is asked for.
class TrivialResolver : public OpResolver {
 public:
  explicit TrivialResolver(TfLiteRegistration* constant_return = nullptr)
      : constant_return_(constant_return) {}
  // Find the op registration of a custom operator by op name.
  const TfLiteRegistration* FindOp(tflite::BuiltinOperator op,
                                   int version) const override {
    return constant_return_;
  }
  // Find the op registration of a custom operator by op name.
  const TfLiteRegistration* FindOp(const char* op, int version) const override {
    return constant_return_;
  }

 private:
  TfLiteRegistration* constant_return_;
};

TEST(BasicFlatBufferModel, TestNonExistantFiles) {
  ASSERT_TRUE(!FlatBufferModel::BuildFromFile("/tmp/tflite_model_1234"));
}

// Make sure a model with nothing in it loads properly.
TEST(BasicFlatBufferModel, TestEmptyModelsAndNullDestination) {
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/empty_model.bin");
  ASSERT_TRUE(model);
  // Now try to build it into a model.
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(InterpreterBuilder(*model, TrivialResolver())(&interpreter),
            kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_NE(InterpreterBuilder(*model, TrivialResolver())(nullptr), kTfLiteOk);
}

// Make sure currently unsupported # of subgraphs are checked
// TODO(aselle): Replace this test when multiple subgraphs are supported.
TEST(BasicFlatBufferModel, TestZeroSubgraphs) {
  auto m = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/0_subgraphs.bin");
  ASSERT_TRUE(m);
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_NE(InterpreterBuilder(*m, TrivialResolver())(&interpreter), kTfLiteOk);
}

TEST(BasicFlatBufferModel, TestMultipleSubgraphs) {
  auto m = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/2_subgraphs.bin");
  ASSERT_TRUE(m);
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(InterpreterBuilder(*m, TrivialResolver())(&interpreter), kTfLiteOk);
  EXPECT_EQ(interpreter->subgraphs_size(), 2);
}

// Test what happens if we cannot bind any of the ops.
TEST(BasicFlatBufferModel, TestModelWithoutNullRegistrations) {
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/test_model.bin");
  ASSERT_TRUE(model);
  // Check that we get an error code and interpreter pointer is reset.
  std::unique_ptr<Interpreter> interpreter(new Interpreter);
  ASSERT_NE(InterpreterBuilder(*model, TrivialResolver(nullptr))(&interpreter),
            kTfLiteOk);
  ASSERT_EQ(interpreter, nullptr);
}

// Make sure model is read to interpreter propelrly
TEST(BasicFlatBufferModel, TestModelInInterpreter) {
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/test_model.bin");
  ASSERT_TRUE(model);
  // Check that we get an error code and interpreter pointer is reset.
  std::unique_ptr<Interpreter> interpreter(new Interpreter);
  ASSERT_EQ(
      InterpreterBuilder(*model, TrivialResolver(&dummy_reg))(&interpreter),
      kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_EQ(interpreter->tensors_size(), 4);
  ASSERT_EQ(interpreter->nodes_size(), 2);
  std::vector<int> inputs = {0, 1};
  std::vector<int> outputs = {2, 3};
  ASSERT_EQ(interpreter->inputs(), inputs);
  ASSERT_EQ(interpreter->outputs(), outputs);

  EXPECT_EQ(std::string(interpreter->GetInputName(0)), "input0");
  EXPECT_EQ(std::string(interpreter->GetInputName(1)), "input1");
  EXPECT_EQ(std::string(interpreter->GetOutputName(0)), "out1");
  EXPECT_EQ(std::string(interpreter->GetOutputName(1)), "out2");

  // Make sure all input tensors are correct
  TfLiteTensor* i0 = interpreter->tensor(0);
  ASSERT_EQ(i0->type, kTfLiteFloat32);
  ASSERT_NE(i0->data.raw, nullptr);  // mmapped
  ASSERT_EQ(i0->allocation_type, kTfLiteMmapRo);
  TfLiteTensor* i1 = interpreter->tensor(1);
  ASSERT_EQ(i1->type, kTfLiteFloat32);
  ASSERT_EQ(i1->data.raw, nullptr);
  ASSERT_EQ(i1->allocation_type, kTfLiteArenaRw);
  TfLiteTensor* o0 = interpreter->tensor(2);
  ASSERT_EQ(o0->type, kTfLiteFloat32);
  ASSERT_EQ(o0->data.raw, nullptr);
  ASSERT_EQ(o0->allocation_type, kTfLiteArenaRw);
  TfLiteTensor* o1 = interpreter->tensor(3);
  ASSERT_EQ(o1->type, kTfLiteFloat32);
  ASSERT_EQ(o1->data.raw, nullptr);
  ASSERT_EQ(o1->allocation_type, kTfLiteArenaRw);

  // Check op 0 which has inputs {0, 1} outputs {2}.
  {
    const std::pair<TfLiteNode, TfLiteRegistration>* node_and_reg0 =
        interpreter->node_and_registration(0);
    ASSERT_NE(node_and_reg0, nullptr);
    const TfLiteNode& node0 = node_and_reg0->first;
    const TfLiteRegistration& reg0 = node_and_reg0->second;
    TfLiteIntArray* desired_inputs = TfLiteIntArrayCreate(2);
    desired_inputs->data[0] = 0;
    desired_inputs->data[1] = 1;
    TfLiteIntArray* desired_outputs = TfLiteIntArrayCreate(1);
    desired_outputs->data[0] = 2;
    ASSERT_TRUE(TfLiteIntArrayEqual(node0.inputs, desired_inputs));
    ASSERT_TRUE(TfLiteIntArrayEqual(node0.outputs, desired_outputs));
    TfLiteIntArrayFree(desired_inputs);
    TfLiteIntArrayFree(desired_outputs);
    ASSERT_EQ(reg0, dummy_reg);
  }

  // Check op 1 which has inputs {2} outputs {3}.
  {
    const std::pair<TfLiteNode, TfLiteRegistration>* node_and_reg1 =
        interpreter->node_and_registration(1);
    ASSERT_NE(node_and_reg1, nullptr);
    const TfLiteNode& node1 = node_and_reg1->first;
    const TfLiteRegistration& reg1 = node_and_reg1->second;
    TfLiteIntArray* desired_inputs = TfLiteIntArrayCreate(1);
    TfLiteIntArray* desired_outputs = TfLiteIntArrayCreate(1);
    desired_inputs->data[0] = 2;
    desired_outputs->data[0] = 3;
    ASSERT_TRUE(TfLiteIntArrayEqual(node1.inputs, desired_inputs));
    ASSERT_TRUE(TfLiteIntArrayEqual(node1.outputs, desired_outputs));
    TfLiteIntArrayFree(desired_inputs);
    TfLiteIntArrayFree(desired_outputs);
    ASSERT_EQ(reg1, dummy_reg);
  }
}

// This tests on a flatbuffer that defines a shape of 2 to be a memory mapped
// buffer. But the buffer is provided to be only 1 element.
TEST(BasicFlatBufferModel, TestBrokenMmap) {
  ASSERT_FALSE(FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/test_model_broken.bin"));
}

TEST(BasicFlatBufferModel, TestNullModel) {
  // Check that we get an error code and interpreter pointer is reset.
  std::unique_ptr<Interpreter> interpreter(new Interpreter);
  ASSERT_NE(
      InterpreterBuilder(nullptr, TrivialResolver(&dummy_reg))(&interpreter),
      kTfLiteOk);
  ASSERT_EQ(interpreter.get(), nullptr);
}

// Mocks the verifier by setting the result in ctor.
class FakeVerifier : public tflite::TfLiteVerifier {
 public:
  explicit FakeVerifier(bool result) : result_(result) {}
  bool Verify(const char* data, int length,
              tflite::ErrorReporter* reporter) override {
    return result_;
  }

 private:
  bool result_;
};

// This makes sure the ErrorReporter is marshalled from FlatBufferModel to
// the Interpreter.
TEST(BasicFlatBufferModel, TestCustomErrorReporter) {
  TestErrorReporter reporter;
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/empty_model.bin",
      &reporter);
  ASSERT_TRUE(model);

  std::unique_ptr<Interpreter> interpreter;
  TrivialResolver resolver;
  InterpreterBuilder(*model, resolver)(&interpreter);
  ASSERT_NE(interpreter->Invoke(), kTfLiteOk);
  ASSERT_EQ(reporter.num_calls(), 1);
}

// This makes sure the ErrorReporter is marshalled from FlatBufferModel to
// the Interpreter.
TEST(BasicFlatBufferModel, TestNullErrorReporter) {
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/empty_model.bin", nullptr);
  ASSERT_TRUE(model);

  std::unique_ptr<Interpreter> interpreter;
  TrivialResolver resolver;
  InterpreterBuilder(*model, resolver)(&interpreter);
  ASSERT_NE(interpreter->Invoke(), kTfLiteOk);
}

// Test that loading model directly from a Model flatbuffer works.
TEST(BasicFlatBufferModel, TestBuildFromModel) {
  TestErrorReporter reporter;
  FileCopyAllocation model_allocation(
      "tensorflow/lite/testdata/test_model.bin", &reporter);
  ASSERT_TRUE(model_allocation.valid());
  ::flatbuffers::Verifier verifier(
      reinterpret_cast<const uint8_t*>(model_allocation.base()),
      model_allocation.bytes());
  ASSERT_TRUE(VerifyModelBuffer(verifier));
  const Model* model_fb = ::tflite::GetModel(model_allocation.base());

  auto model = FlatBufferModel::BuildFromModel(model_fb);
  ASSERT_TRUE(model);

  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(
      InterpreterBuilder(*model, TrivialResolver(&dummy_reg))(&interpreter),
      kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
}

// TODO(aselle): Add tests for serialization of builtin op data types.
// These tests will occur with the evaluation tests of individual operators,
// not here.

}  // namespace tflite

FILE* openReadFileGetSize(const char* fname, unsigned long* sizep) {
  FILE* fp;
  if ((fp = fopen(fname, "rb")) == NULL) {
    printf("### fatal error: cannot open '%s'\n", fname);
    return 0;
  }
  if (sizep != NULL) {
    fseek(fp, 0, SEEK_END);
    *sizep = ftell(fp);
    fseek(fp, 0, SEEK_SET);
  }
  return fp;
}


void readNPYFileData(const char* fname, void* buffer,
                              unsigned long size) {
  unsigned long len;
  FILE* fp = openReadFileGetSize(fname, &len);

  /// \todo we should check the numpy header

  //int sz224 = 224 * 224 * 3;
  // Data is at the end of file, so seek past header
  fseek(fp, len - size, SEEK_SET);
  if (fread(buffer, 1, size, fp) != size) {
    printf("### fatal error: read error file '%s'\n", fname);
  }

  fclose(fp);
}


int main(int argc, char** argv) {
  //std::unique_ptr<tflite::FlatBufferModel> 
      auto model =
      tflite::FlatBufferModel::BuildFromFile(
		  argv[1]
		  //"mobilenetv1/mobilenet_v1_1.0_224_quant.tflite"
		  //"inceptionv1/inception_v1_224_quant.tflite"
    		//  "inceptionv2/inception_v2_224_quant.tflite"
    		//  "inceptionv3/inception_v3_quant.tflite"
    		//  "inceptionv4/inception_v4_299_quant.tflite"
		  //"mobilenetv2/mobilenet_v2_1.0_224_quant.tflite"
      );
  std::unique_ptr<tflite::Interpreter> interpreter;
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder(*model, resolver)(&interpreter);
  TfLiteStatus status = interpreter->AllocateTensors();
  interpreter->SetNumThreads(1);
  uint8_t* input = interpreter->typed_input_tensor<uint8_t>(0);
  // load from numpy file an image ... cat here
  //unsigned long sz = 224 * 224 * 3; //for inception v1, v2
  if (argc < 4 ) { 
	printf( "requries model_path image_path image_height parameters\n"); 
	exit(1);
  }
	std::string arg = argv[3];
   std::size_t pos;
  int height = std::stoi(arg, &pos); 
				  
  unsigned long sz = height * height * 3; // for inception v3, v4
  //readNPYFileData("/home/jay/mobilenet_models/mobilenet_v1_1.0_224_quant/cat224ui8tf.npy", input, sz);
  readNPYFileData(argv[2], input, sz);
  status = interpreter->Invoke();
  uint8_t* output = interpreter->typed_output_tensor<uint8_t>(0);
  //::tflite::LogToStderr();
  //::testing::InitGoogleTest(&argc, argv);
 // return RUN_ALL_TESTS();
}
