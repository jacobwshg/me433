import tkinter as tk
import math
import serial
import threading
import time

# --- CONFIGURATION ---
SERIAL_PORT = 'COM3'  # Change to your port (e.g., '/dev/ttyUSB0' on Linux/Mac)
BAUD_RATE = 9600
MAX_DATA_VALUE = 4095

class Gauge:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Data Gauge")
        self.root.configure(bg='black')
        
        # Window dimensions
        self.size = 600
        self.center = self.size // 2
        self.radius = self.size // 2 - 20
        
        # Canvas setup
        self.canvas = tk.Canvas(root, width=self.size, height=self.size, bg='black', highlightthickness=0)
        self.canvas.pack()
        
        # Draw static outer circle
        self.canvas.create_oval(
            self.center - self.radius, self.center - self.radius,
            self.center + self.radius, self.center + self.radius,
            outline='white', width=4
        )
        
        # Initialize arrow ID
        self.arrow = None
        
        # Shared variable for sensor data
        self.current_value = 0
        self.running = True
        
        # Start with default position (0)
        self.update_arrow(0)
        
        # Start background serial reading thread
        self.serial_thread = threading.Thread(target=self.read_serial, daemon=True)
        self.serial_thread.start()
        
        # Start GUI update loop
        self.update_loop()

    def update_arrow(self, value):
        """Calculates and redraws the arrow based on the input value."""
        if self.arrow:
            self.canvas.delete(self.arrow)
            
        # Map 0 -> 4095 to 0 -> 360 degrees
        # Subtract from 90 to start at 12 o'clock, multiply by -1 for clockwise rotation
        angle_degrees = 90 - (value / MAX_DATA_VALUE * 360)
        angle_radians = math.radians(angle_degrees)
        
        # Arrow geometry calculations
        arrow_length = self.radius * 0.75
        head_length = 40
        barb_width = 15
        shaft_width = 10
        
        # Tip of the arrow
        x_tip = self.center + arrow_length * math.cos(angle_radians)
        y_tip = self.center - arrow_length * math.sin(angle_radians)
        
        # Base of the arrow head
        x_head_base = self.center + (arrow_length - head_length) * math.cos(angle_radians)
        y_head_base = self.center - (arrow_length - head_length) * math.sin(angle_radians)
        
        # Perpendicular offsets for arrow geometry
        perp_angle = angle_radians + math.pi / 2
        
        # Arrow head left/right barbs
        x_left_barb = x_head_base + barb_width * math.cos(perp_angle)
        y_left_barb = y_head_base - barb_width * math.sin(perp_angle)
        
        x_right_barb = x_head_base - barb_width * math.cos(perp_angle)
        y_right_barb = y_head_base - barb_width * math.sin(perp_angle)
        
        # Arrow shaft left/right origins (near center)
        x_left_base = self.center + (shaft_width / 2) * math.cos(perp_angle)
        y_left_base = self.center - (shaft_width / 2) * math.sin(perp_angle)
        
        x_right_base = self.center - (shaft_width / 2) * math.cos(perp_angle)
        y_right_base = self.center - (shaft_width / 2) * math.sin(perp_angle)
        
        # Arrow shaft intersections with the head base
        x_left_shaft_top = x_head_base + (shaft_width / 2) * math.cos(perp_angle)
        y_left_shaft_top = y_head_base - (shaft_width / 2) * math.sin(perp_angle)
        
        x_right_shaft_top = x_head_base - (shaft_width / 2) * math.cos(perp_angle)
        y_right_shaft_top = y_head_base - (shaft_width / 2) * math.sin(perp_angle)
        
        # Draw the complete arrow polygon
        points = [
            x_left_base, y_left_base,          # Bottom left
            x_left_shaft_top, y_left_shaft_top,  # Shaft top left
            x_left_barb, y_left_barb,          # Left barb tip
            x_tip, y_tip,                      # Arrow tip
            x_right_barb, y_right_barb,        # Right barb tip
            x_right_shaft_top, y_right_shaft_top,# Shaft top right
            x_right_base, y_right_base         # Bottom right
        ]
        
        self.arrow = self.canvas.create_polygon(points, fill='black', outline='white', width=3)

    def read_serial(self):
        """Thread worker to continuously pull data from the serial port."""
        try:
            with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) as ser:
                while self.running:
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        try:
                            # Safely parse and clamp the value between 0 and 4095
                            val = int(line)
                            self.current_value = max(0, min(val, MAX_DATA_VALUE))
                        except ValueError:
                            pass # Ignore malformed serial strings
                    time.sleep(0.01)
        except serial.SerialException as e:
            print(f"Serial Error: {e}")
            print("Running in simulation mode (arrow will wander automatically).")
            # Simulation fallback if no device is connected
            sim_val = 0
            while self.running:
                sim_val = (sim_val + 20) % (MAX_DATA_VALUE + 1)
                self.current_value = sim_val
                time.sleep(0.03)

    def update_loop(self):
        """Refreshes the canvas drawing at ~60 FPS."""
        self.update_arrow(self.current_value)
        if self.running:
            self.root.after(16, self.update_loop)

    def close(self):
        self.running = False
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = Gauge(root)
    root.protocol("WM_DELETE_WINDOW", app.close)
    root.mainloop()
