#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include "RobotArm.h"
#include "TrajectoryPlanner.h"
#include "MLSolver.h"

using namespace std;

double calculateError(const Eigen::Vector3d& target, const Eigen::Vector3d& actual) {
    return (target - actual).norm();
}

int main() {
    RobotArm arm;
    
    MLSolver ml_solver(L"models/ik_model.onnx"); 

    // Define targets
    vector<Eigen::Vector3d> targets = {
        {0.3, 0.0, 0.4},  
        {0.2, 0.2, 0.3},  
        {0.4, -0.1, 0.2}, 
        {0.6, 0.0, 0.5}   
    };

    cout << "=== Part 1: Performance Comparison ===\n" << endl;

    for (const auto& target : targets) {
        cout << "Target: [" << target.transpose() << "]" << endl;


        auto start_algo = chrono::high_resolution_clock::now();
        JointConfig algo_sol;
        bool algo_success = arm.solveIKAnalytical(target, algo_sol);
        auto end_algo = chrono::high_resolution_clock::now();
        double time_algo = chrono::duration<double, micro>(end_algo - start_algo).count();

        if (algo_success) {
            Eigen::Vector3d actual = arm.forwardKinematics(algo_sol);
            cout << "[Algo] Time: " << time_algo << " us | Error: " << calculateError(target, actual) << " m" << endl;
        } else {
            cout << "[Algo] Failed (Unreachable)" << endl;
        }


        try {
            auto start_ml = chrono::high_resolution_clock::now();
            JointConfig ml_sol = ml_solver.solve((float)target.x(), (float)target.y(), (float)target.z());
            auto end_ml = chrono::high_resolution_clock::now();
            double time_ml = chrono::duration<double, micro>(end_ml - start_ml).count();
            Eigen::Vector3d actual_ml = arm.forwardKinematics(ml_sol);
            cout << "[ML  ] Time: " << time_ml << " us | Error: " << calculateError(target, actual_ml) << " m" << endl;
        } catch (const std::exception& e) {
            cout << "[ML  ] Error: " << e.what() << endl;
        }
        cout << "-----------------------------------" << endl;
    }


    cout << "\n=== Part 2: Generating Trajectory Log ===" << endl;

    ofstream log_file("trajectory_log.csv");
    log_file << "x,y,z,t1,t2,t3,method\n";


    Eigen::Vector3d current_pos = {0.3, 0.0, 0.4}; 

    for (const auto& target : targets) {
        cout << "Planning path to [" << target.transpose() << "]... ";


        auto path = TrajectoryPlanner::generatePath(current_pos, target, 20);

        bool path_possible = true;
        
        for (const auto& point : path) {
            JointConfig q;
            if (arm.solveIKAnalytical(point, q)) {
                log_file << point.x() << "," << point.y() << "," << point.z() << ","
                         << q.theta1 << "," << q.theta2 << "," << q.theta3 << ",Algo\n";
            } else {
                path_possible = false;
            }
        }
        
        if (path_possible) {
            cout << "Done." << endl;
            current_pos = target;
        } else {
            cout << "Skipped (Unreachable)." << endl;
        }
    }
    
    log_file.close();
    cout << "Trajectory logged to 'trajectory_log.csv'. Run visualization script now." << endl;


    cout << "\nPress Enter to exit...";
    cin.get();
    return 0;
}