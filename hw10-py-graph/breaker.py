
import pgzrun
import pygame
import serial
from pygame import Rect

# --- Configuration ---
WIDTH = 710
HEIGHT = 710

# --- Serial Port Setup ---
# Update 'COM3' (Windows) or '/dev/ttyUSB0' (Mac/Linux) to match your device.
SERIAL_PORT = 'COM3'
BAUD_RATE = 9600

try:
    # timeout=0 ensures non-blocking reads so the game loop doesn't freeze
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0)
    use_serial = True
    print(f"Connected to {SERIAL_PORT}")
except Exception as e:
    print(f"Could not connect to {SERIAL_PORT}. Falling back to Left/Right arrow keys.")
    use_serial = False

# --- Game Objects ---
paddle = Rect((WIDTH // 2 - 50, HEIGHT - 40), (100, 20))

# Ball variables
ball_pos = [WIDTH // 2, HEIGHT - 70]
ball_vel = [5, -5]
ball_radius = 10

# Bricks
bricks = []
brick_colors = []
# Colors based on your image
colors = [(255, 0, 0),    # Red
          (255, 128, 0),  # Orange
          (255, 255, 0),  # Yellow
          (0, 128, 255),  # Blue
          (255, 0, 255),  # Magenta
          (0, 255, 0)]    # Green

# Generate 6 rows, 7 columns
rows, cols = 6, 7
brick_width = 90
brick_height = 30
padding = 5
offset_x = (WIDTH - (cols * (brick_width + padding))) // 2
offset_y = 50

for row in range(rows):
    for col in range(cols):
        x = offset_x + col * (brick_width + padding)
        y = offset_y + row * (brick_height + padding)
        bricks.append(Rect((x, y), (brick_width, brick_height)))
        brick_colors.append(colors[row])

def update():
    global ball_vel, use_serial
    
    # 1. Fetch Input (Serial or mouse)
    if use_serial and ser.in_waiting > 0:
        try:
            # Read line, decode, and strip whitespace
            line = ser.readline().decode('utf-8').strip()
            if line:
                # Example: Arduino sends an analog pot value (0 to 1023)
                # We map this 0-1023 range to the width of the screen
                pot_val = int(line)
                paddle.centerx = (pot_val / 1023.0) * WIDTH
        except ValueError:
            # Ignore garbled serial data
            pass
    else:
        # Fallback to mouse control if serial is not available
        mouse_x = pygame.mouse.get_pos()[0]
        paddle.centerx = mouse_x

    # Keep paddle within screen bounds
    if paddle.left < 0: paddle.left = 0
    if paddle.right > WIDTH: paddle.right = WIDTH

    # 2. Update Ball Position
    ball_pos[0] += ball_vel[0]
    ball_pos[1] += ball_vel[1]

    # Create a Rect representation of the ball for collision detection
    ball_rect = Rect(
        (ball_pos[0] - ball_radius, ball_pos[1] - ball_radius), 
        (ball_radius * 2, ball_radius * 2)
    )

    # 3. Wall Collisions
    if ball_pos[0] <= ball_radius or ball_pos[0] >= WIDTH - ball_radius:
        ball_vel[0] = -ball_vel[0] # Reverse X
    if ball_pos[1] <= ball_radius:
        ball_vel[1] = -ball_vel[1] # Reverse Y

    # 4. Paddle Collision
    if ball_rect.colliderect(paddle) and ball_vel[1] > 0:
        # Simple bounce
        ball_vel[1] = -ball_vel[1]
        
        # Add slight english/spin based on where it hits the paddle
        hit_pos = (ball_pos[0] - paddle.centerx) / (paddle.width / 2)
        ball_vel[0] = hit_pos * 6 

    # 5. Brick Collision
    hit_index = ball_rect.collidelist(bricks)
    if hit_index != -1:
        # Remove the brick and its color
        bricks.pop(hit_index)
        brick_colors.pop(hit_index)
        # Reverse Y velocity
        ball_vel[1] = -ball_vel[1]

    # 6. Fall off the bottom (Reset)
    if ball_pos[1] > HEIGHT:
        ball_pos[0] = WIDTH // 2
        ball_pos[1] = HEIGHT - 70
        ball_vel[1] = -5

def draw():
    screen.clear()
    # Dark blue background with slight visual depth
    screen.fill((0, 0, 80)) 

    # Draw bricks with a black outline to match the image style
    for i, b in enumerate(bricks):
        screen.draw.filled_rect(b, brick_colors[i])
        screen.draw.rect(b, (0, 0, 0)) 

    # Draw paddle (gray center, orange edges matching the reference roughly)
    screen.draw.filled_rect(paddle, (150, 150, 150))
    screen.draw.filled_rect(Rect((paddle.x, paddle.y), (15, 20)), (255, 100, 0))
    screen.draw.filled_rect(Rect((paddle.right - 15, paddle.y), (15, 20)), (255, 100, 0))

    # Draw ball (cyan with white outline)
    screen.draw.filled_circle((int(ball_pos[0]), int(ball_pos[1])), ball_radius + 2, (255, 255, 255))
    screen.draw.filled_circle((int(ball_pos[0]), int(ball_pos[1])), ball_radius, (0, 255, 255))

pgzrun.go()

