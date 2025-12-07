import matplotlib.pyplot as plt

# Read ms per frame from benchmark.txt, skipping the first line
with open('benchmark.txt', 'r') as f:
    lines = f.readlines()[10:]  # Skip the first line
    ms_list = [float(line.strip()) for line in lines if line.strip()]

# Calculate cumulative time in seconds for each frame
time_axis = []
current_time = 0.0
for ms in ms_list:
    current_time += ms / 1000.0
    time_axis.append(current_time)

# Plot ms per frame with time axis
plt.figure(figsize=(10, 5))
plt.plot(time_axis, ms_list, label='ms per frame')
plt.xlabel('Time (s)')
plt.ylabel('Milliseconds')
plt.title('Frame Time (ms) Over Time')
plt.legend()
plt.tight_layout()
plt.savefig('ms_per_frame.png')
plt.close()

# Convert ms to FPS
fps_list = [1000.0 / ms if ms > 0 else 0 for ms in ms_list]

# Plot FPS with time axis
plt.figure(figsize=(10, 5))
plt.plot(time_axis, fps_list, label='FPS')
plt.xlabel('Time (s)')
plt.ylabel('Frames Per Second')
plt.title('FPS Over Time')
plt.legend()
plt.tight_layout()
plt.savefig('fps_plot.png')
plt.close()