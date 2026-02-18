#include "RobotArm.h"
#include <algorithm>

Eigen::Vector3d RobotArm::forwardKinematics(const JointConfig& q) {
    double r_plane = L2 * std::cos(q.theta2) + L3 * std::cos(q.theta2 + q.theta3);
    
    double x = std::cos(q.theta1) * r_plane;
    double y = std::sin(q.theta1) * r_plane;
    double z = L1 + L2 * std::sin(q.theta2) + L3 * std::sin(q.theta2 + q.theta3);
    
    return Eigen::Vector3d(x, y, z);
}

bool RobotArm::solveIKAnalytical(const Eigen::Vector3d& target, JointConfig& sol) {
    sol.theta1 = std::atan2(target.y(), target.x());

    double r_plane = std::sqrt(target.x()*target.x() + target.y()*target.y());
    double z_offset = target.z() - L1;
    
    double D_sq = r_plane * r_plane + z_offset * z_offset;
    double D = std::sqrt(D_sq);

    if (D > (L2 + L3) || D < std::abs(L2 - L3)) {
        return false;
    }

    double cos_theta3 = (D_sq - L2*L2 - L3*L3) / (2 * L2 * L3);
    
    if (cos_theta3 > 1.0) cos_theta3 = 1.0;
    if (cos_theta3 < -1.0) cos_theta3 = -1.0;
    
    sol.theta3 = -std::acos(cos_theta3);

    double alpha = std::atan2(z_offset, r_plane);
    double beta = std::atan2(L3 * std::sin(sol.theta3), L2 + L3 * std::cos(sol.theta3));
    sol.theta2 = alpha - beta;

    return checkLimits(sol);
}

bool RobotArm::checkLimits(const JointConfig& config) {
    return (config.theta1 >= LIMIT_MIN && config.theta1 <= LIMIT_MAX &&
            config.theta2 >= LIMIT_MIN && config.theta2 <= LIMIT_MAX &&
            config.theta3 >= LIMIT_MIN && config.theta3 <= LIMIT_MAX);
}