# Robotic Arm Control System (Inverse Kinematics)

## Overview
This project is a technical assessment for the **Machine Learning Engineer Intern** position. It implements a control system for a 3-DOF robotic arm using two distinct approaches:
1.  **Algorithmic Solver:** An analytical inverse kinematics (IK) solver using geometric methods.
2.  **Machine Learning Solver:** A neural network-based solver trained on workspace data, deployed using ONNX Runtime in C++.

The system is implemented in **C++17** for real-time performance, with **Python** used for data generation, model training, and trajectory visualization.

## Features
* **Real-time C++ Control Loop:** Runs under 1ms per cycle.
* **Hybrid Architecture:** Seamlessly switches between Analytical and ML solvers.
* **Embedded Normalization:** The ONNX model includes baked-in data preprocessing, ensuring 100% consistency between Python training and C++ inference.
* **Trajectory Planning:** Generates smooth linear paths between targets.
* **Visualization:** Python script to visualize the robot's motion from C++ logs.

## Directory Structure
```text
/robotic_arm_assessment
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files (RobotArm.h, MLSolver.h)
├── src/                    # Source code (main.cpp, Logic)
├── scripts/                # Python scripts (Training & Viz)
│   ├── generate_data.py
│   ├── train_model.py
│   └── visualize_arm.py
├── models/                 # Stores the trained .onnx model
```

## Prerequisites 

* **OS:** Windows 10/11 (Project configured for Visual Studio 2022).
* **C++ Compiler:** MSVC (Visual Studio) with C++17 support.
* **CMake:** Version 3.14 or higher.
* **Python:** 3.8+ with pip.

## Setup Instructions

### 1. Library Installation
This project uses a local libs folder to manage dependencies without system-wide installation.

Eigen: Download and extract the Eigen source folder into libs/Eigen.

ONNX Runtime: Download the Windows x64 release. Extract it so that libs/onnxruntime contains include/ and lib/ folders.

### 2. Python Environment & Model Training
Before running the C++ code, you must generate the dataset and train the neural network.

Install Dependencies:
```bash
pip install -r scripts/requirements.txt
```
Generate Data:
Creates a consistent dataset of 50,000 "Elbow Down" configuration samples.
```bash
python scripts/generate_data.py
```
Train Model:
Trains the PyTorch model and exports models/ik_model.onnx.
Note: This script automatically validates the model accuracy in Python.
```bash
python scripts/train_model.py
```

### 3. Building the C++ Project

Open Visual Studio 2022.
Select "Open a local folder" and choose the robotic_arm_assessment directory.
Visual Studio will automatically configure CMake.
If it fails, ensure the libs/ folder structure is correct.
Go to Build > Rebuild All.

## Running the Application

### 1. Performance Benchmark
Run the executable (robot_control.exe) from Visual Studio (Green Play Button).
The console will output a comparison of the two approaches:

```plaintext
=== Robotic Arm IK Assessment ===
Target: [0.3, 0, 0.4]
[Algo] Time: 40.6 us | Error: 0 m
[ML  ] Time: 237.7 us | Error: 0.0034 m  <-- High Accuracy!
```

### 2. Trajectory Visualization
The C++ program generates a log file named trajectory_log.csv in the build directory. 
To view the motion:
* Open a terminal in the scripts/ folder.
* Run the visualizer (update the path to the csv if necessary):
```bash
python visualize_arm.py
```
* A 3D animation of the arm reaching the targets will appear.
