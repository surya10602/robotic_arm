import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation


L1, L2, L3 = 0.3, 0.25, 0.15

def forward_kinematics_chain(t1, t2, t3):

    x0, y0, z0 = 0, 0, 0
    x1, y1, z1 = 0, 0, L1
    
    r2 = L2 * np.cos(t2)
    x2 = np.cos(t1) * r2
    y2 = np.sin(t1) * r2
    z2 = z1 + L2 * np.sin(t2)
    
    r3 = L2 * np.cos(t2) + L3 * np.cos(t2 + t3)
    x3 = np.cos(t1) * r3
    y3 = np.sin(t1) * r3
    z3 = z1 + L2 * np.sin(t2) + L3 * np.sin(t2 + t3)
    
    return [x0, x1, x2, x3], [y0, y1, y2, y3], [z0, z1, z2, z3]


df = pd.read_csv('../build/trajectory_log.csv')

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.set_xlim(-0.5, 0.5); ax.set_ylim(-0.5, 0.5); ax.set_zlim(0, 0.8)

line, = ax.plot([], [], [], 'o-', lw=2)

def update(frame):
    row = df.iloc[frame]
    xs, ys, zs = forward_kinematics_chain(row['t1'], row['t2'], row['t3'])
    line.set_data(xs, ys)
    line.set_3d_properties(zs)
    ax.set_title(f"Target: {row['x']:.2f}, {row['y']:.2f}, {row['z']:.2f}")
    return line,

ani = FuncAnimation(fig, update, frames=len(df), interval=100)
plt.show()