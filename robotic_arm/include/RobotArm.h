#pragma once
#include <Eigen/Dense>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct JointConfig {
    double theta1;
    double theta2;
    double theta3;
};

class RobotArm {
private:
    const double L1 = 0.30;
    const double L2 = 0.25;
    const double L3 = 0.15;

    const double LIMIT_MIN = -M_PI; 
    const double LIMIT_MAX = M_PI;

public:
    RobotArm() = default;

    Eigen::Vector3d forwardKinematics(const JointConfig& joints);

    bool solveIKAnalytical(const Eigen::Vector3d& target, JointConfig& solution);

    bool checkLimits(const JointConfig& config);
};