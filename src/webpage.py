from flask import Flask, render_template, Response, request, jsonify
import cv2
import threading
import time
from functionality import *  # Ensure functionality.py contains the necessary mode functions
from threading import Lock

# Global variables
joy_data = [0, 0]
joy_data_lock = Lock()
# mode_check = 

app = Flask(__name__, template_folder="public", static_folder="public")

# Capture from webcam
camera = cv2.VideoCapture(0)

cam_read = None

# Global variables for joystick data and current mode
joy_data = [0, 0]
current_mode = "idle"  # Start in idle mode

# Function to process frames for 'follow_me' mode
def generate_frames():
    global current_mode, cam_read
    while True:
        success, frame = camera.read()
        frame = cv2.resize(frame, (640, 480))
        if not success:
            break

        # Process the frame using CUDA
        if current_mode == "follow_me":
            # Convert frame to GPU memory
            gpu_frame = cv2.cuda_GpuMat()
            gpu_frame.upload(frame)
            
            # Example of CUDA operation: Convert to HSV (GPU-based operation)
            # hsv_gpu_frame = cv2.cuda.cvtColor(gpu_frame, cv2.COLOR_BGR2HSV)

            # Example of other CUDA operations like resizing (you can add more operations)
            # resized_gpu_frame = cv2.cuda.resize(gpu_frame, (640, 480))

            # Download processed frame back to CPU
            # cam_read = resized_gpu_frame.download()
            cam_read = gpu_frame.download()

        else:
            # If not in follow_me mode, use the original frame
            cam_read = frame

        # Encode the processed frame to send over HTTP
        ret, buffer = cv2.imencode('.jpg', cam_read)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

# Background thread to handle mode checking and execution
def mode_check_loop():
    global cam_read, current_mode
    # print("inside mode check")
    while True:
        
        # print("inside mode check")
        with joy_data_lock:
            current_joy_data = joy_data.copy()  # Copy the data to avoid race condition

        if current_mode == "remote_control":
            # Call remote_control function using joystick_data
            remote_control(current_joy_data)
        elif current_mode == "learn":
            # Call learn function
            learn(current_joy_data)
        elif current_mode == "repeat":
            # Call repeat function
            repeat()
        elif current_mode == "follow_me":
            # Process follow_me mode in video frames
            if cam_read is not None:
                follow_me(cam_read)

        time.sleep(0.040)  # Small delay to prevent overloading the CPU

# Flask Routes
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/action/remote_control', methods=['POST'])
def remote_control_action():
    global current_mode
    current_mode = "remote_control"
    return jsonify({"message": "Remote Control activated"})

@app.route('/action/learn', methods=['POST'])
def learn_action():
    global current_mode
    current_mode = "learn"
    return jsonify({"message": "Learning mode activated"})

@app.route('/action/repeat', methods=['POST'])
def repeat_action():
    global current_mode
    current_mode = "repeat"
    return jsonify({"message": "Repeating previous actions"})

@app.route('/action/follow_me', methods=['POST'])
def follow_me_action():
    global current_mode
    current_mode = "follow_me"  # Set mode to follow_me
    return jsonify({"message": "Follow Me mode activated"})

@app.route('/action/stop', methods=['POST'])
def stop_action():
    global current_mode
    current_mode = "idle"  # Stop all actions and go idle
    stop()
    return jsonify({"message": "Stop mode activated"})

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/joystick_input', methods=['POST'])
def joystick_input():
    global joy_data, cam_read
    try:
        data = request.json
        x = data.get('x', 0)
        y = data.get('y', 0)

        y = -y  # Invert Y axis to match expected control
        with joy_data_lock:
            joy_data = [x, y]
        print(f"Joystick Input - X: {x}, Y: {y}")
        # if current_mode == "remote_control":
        #     remote_control(joy_data)
        # elif current_mode == "learn":
        #     learn(joy_data)
        # elif current_mode == "repeat":
        #     repeat()
        # elif current_mode == "follow_me":
        #     if cam_read is not None:
        #         follow_me(cam_read)


        return jsonify({"status": "success", "message": f"Joystick data received: X={x}, Y={y}"}), 200
    except Exception as e:
        print(f"Error processing joystick input: {e}")
        return jsonify({"status": "error", "message": str(e)}), 400


if __name__ == '__main__':
    # Start the background thread for mode checking
    thread = threading.Thread(target=mode_check_loop)
    thread.daemon = True  # Ensure the thread exits when the main program exits
    thread.start()

    # Start the Flask app
    app.run(host="0.0.0.0", port=5000)
