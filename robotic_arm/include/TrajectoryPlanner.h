#pragma once
#include <Eigen/Dense>
#include <vector>

struct Obstacle {
    Eigen::Vector3d position;
    double radius;
};

class TrajectoryPlanner {
public:
    static std::vector<Eigen::Vector3d> generatePath(
        const Eigen::Vector3d& start, 
        const Eigen::Vector3d& end, 
        int steps
    );

    static bool checkCollision(const Eigen::Vector3d& point, const std::vector<Obstacle>& obstacles);
};