#include "MLSolver.h"
#include <iostream>

MLSolver::MLSolver(const std::wstring& model_path) 
    : env(ORT_LOGGING_LEVEL_WARNING, "IK_ML"), 
      session(env, model_path.c_str(), Ort::SessionOptions()) 
{
}

JointConfig MLSolver::solve(float x, float y, float z) {
    std::vector<float> input_data = { x, y, z };

    std::vector<int64_t> input_shape = {1, 3};
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, input_data.data(), 3, input_shape.data(), 2);

    const char* input_names[] = {"input"};
    const char* output_names[] = {"output"};
    
    auto output_tensors = session.Run(
        Ort::RunOptions{nullptr}, input_names, &input_tensor, 1, output_names, 1);

    float* out_ptr = output_tensors[0].GetTensorMutableData<float>();
    
    return { (double)out_ptr[0], (double)out_ptr[1], (double)out_ptr[2] };
}