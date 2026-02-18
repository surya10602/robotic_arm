import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
import onnxruntime as ort
import os

L1, L2, L3 = 0.3, 0.25, 0.15
SAMPLES = 50000
EPOCHS = 100 

print(f"Generating {SAMPLES} samples (Front Workspace Only)...")

x = np.random.uniform(0.05, 0.7, SAMPLES) 
y = np.random.uniform(-0.7, 0.7, SAMPLES)
z = np.random.uniform(0.0, 0.8, SAMPLES)

valid_data = []

for i in range(SAMPLES):
    xi, yi, zi = x[i], y[i], z[i]
    
    t1 = np.arctan2(yi, xi)
    
    r_plane = np.sqrt(xi**2 + yi**2)
    z_offset = zi - L1
    D = np.sqrt(r_plane**2 + z_offset**2)
    
    if D > (L2 + L3) or D < abs(L2 - L3):
        continue
        
    cos_t3 = (D**2 - L2**2 - L3**2) / (2 * L2 * L3)
    cos_t3 = np.clip(cos_t3, -1.0, 1.0)
    t3 = -np.arccos(cos_t3)
    
    alpha = np.arctan2(z_offset, r_plane)
    beta = np.arctan2(L3 * np.sin(t3), L2 + L3 * np.cos(t3))
    t2 = alpha - beta
    
    valid_data.append([xi, yi, zi, t1, t2, t3])

data = np.array(valid_data, dtype=np.float32)
print(f"Valid samples kept: {len(data)}")

X_raw = data[:, :3]
y_raw = data[:, 3:]

mean = torch.tensor(X_raw.mean(axis=0), dtype=torch.float32)
scale = torch.tensor(X_raw.std(axis=0), dtype=torch.float32)

print(f"Mean: {mean.numpy()}")
print(f"Scale: {scale.numpy()}")

class IKModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.register_buffer("mean", mean)
        self.register_buffer("scale", scale)
        
        self.net = nn.Sequential(
            nn.Linear(3, 512),
            nn.ReLU(),
            nn.Linear(512, 512),
            nn.ReLU(),
            nn.Linear(512, 512),
            nn.ReLU(),
            nn.Linear(512, 3)
        )

    def forward(self, x):
        x_norm = (x - self.mean) / (self.scale + 1e-6)
        return self.net(x_norm)

model = IKModel()
optimizer = optim.Adam(model.parameters(), lr=0.0005)
criterion = nn.MSELoss()

X_t = torch.tensor(X_raw)
y_t = torch.tensor(y_raw)

print("Training...")
for epoch in range(EPOCHS):
    optimizer.zero_grad()
    outputs = model(X_t)
    loss = criterion(outputs, y_t)
    loss.backward()
    optimizer.step()
    
    if epoch % 10 == 0:
        print(f"Epoch {epoch}: Loss {loss.item():.6f}")

model.eval()
output_path = "models/ik_model.onnx"
os.makedirs("models", exist_ok=True)

dummy_input = torch.randn(1, 3) 
torch.onnx.export(model, dummy_input, output_path, input_names=['input'], output_names=['output'])
print(f"Model exported to {output_path}")

print("\n--- VERIFYING ONNX MODEL IN PYTHON ---")
ort_session = ort.InferenceSession(output_path)

test_target = np.array([[0.3, 0.0, 0.4]], dtype=np.float32)
onnx_inputs = {'input': test_target}
onnx_output = ort_session.run(None, onnx_inputs)[0][0]

t1, t2, t3 = onnx_output
r_p = L2 * np.cos(t2) + L3 * np.cos(t2 + t3)
pred_x = np.cos(t1) * r_p
pred_y = np.sin(t1) * r_p
pred_z = L1 + L2 * np.sin(t2) + L3 * np.sin(t2 + t3)

error = np.linalg.norm(test_target[0] - np.array([pred_x, pred_y, pred_z]))
print(f"Target: {test_target[0]}")
print(f"Predicted Angles: {onnx_output}")
print(f"Result Position: [{pred_x:.4f}, {pred_y:.4f}, {pred_z:.4f}]")
print(f"ERROR: {error:.6f} m")

if error < 0.05:
    print("SUCCESS! The model is accurate. Now run C++.")
else:
    print("WARNING! Model is still inaccurate.")