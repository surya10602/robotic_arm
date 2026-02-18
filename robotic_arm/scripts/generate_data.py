import numpy as np
import pandas as pd

L1, L2, L3 = 0.3, 0.25, 0.15
SAMPLES = 100000  

print(f"Generating {SAMPLES} consistent samples...")

data = []

for _ in range(SAMPLES):
    x = np.random.uniform(-0.7, 0.7)
    y = np.random.uniform(-0.7, 0.7)
    z = np.random.uniform(0.0, 0.8)


    t1 = np.arctan2(y, x)

    r_plane = np.sqrt(x**2 + y**2)
    z_offset = z - L1
    D = np.sqrt(r_plane**2 + z_offset**2)

    if D > (L2 + L3) or D < abs(L2 - L3):
        continue

    cos_t3 = (D**2 - L2**2 - L3**2) / (2 * L2 * L3)
    cos_t3 = np.clip(cos_t3, -1.0, 1.0)
    t3 = -np.arccos(cos_t3)

    alpha = np.arctan2(z_offset, r_plane)
    beta = np.arctan2(L3 * np.sin(t3), L2 + L3 * np.cos(t3))
    t2 = alpha - beta

    data.append([x, y, z, t1, t2, t3])

df = pd.DataFrame(data, columns=['x', 'y', 'z', 't1', 't2', 't3'])
df.to_csv('ik_dataset.csv', index=False)
print(f"Saved {len(df)} samples to ik_dataset.csv")