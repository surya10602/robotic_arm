#include "TrajectoryPlanner.h"

std::vector<Eigen::Vector3d> TrajectoryPlanner::generatePath(
    const Eigen::Vector3d& start, 
    const Eigen::Vector3d& end, 
    int steps) 
{
    std::vector<Eigen::Vector3d> path;
    for (int i = 0; i <= steps; ++i) {
        double t = (double)i / steps;
        Eigen::Vector3d point = start + t * (end - start);
        path.push_back(point);
    }
    return path;
}

bool TrajectoryPlanner::checkCollision(const Eigen::Vector3d& point, const std::vector<Obstacle>& obstacles) {
    for (const auto& obs : obstacles) {
        if ((point - obs.position).norm() < obs.radius) {
            return true; 
        }
    }
    return false;
}