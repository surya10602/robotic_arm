#pragma once
#include <onnxruntime_cxx_api.h>
#include <vector>
#include <string>
#include "RobotArm.h"

class MLSolver {
private:
    Ort::Env env;
    Ort::Session session;
    

public:
    MLSolver(const std::wstring& model_path);
    JointConfig solve(float x, float y, float z);
};