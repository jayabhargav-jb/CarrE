from flask import Flask, render_template, Response, jsonify
import cv2

app = Flask(__name__, template_folder="public", static_folder="public")

# Capture from webcam
camera = cv2.VideoCapture(0)

def generate_frames():
    while True:
        success, frame = camera.read()
        if not success:
            break
        else:
            # Encode the frame in JPEG format
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/action/<action_name>', methods=['POST'])
def perform_action(action_name):
    # Placeholder for button functionality
    if action_name == "remote_control":
        message = "Remote Control action executed!"
    elif action_name == "learn":
        message = "Learn action executed!"
    elif action_name == "repeat":
        message = "Repeat action executed!"
    elif action_name == "follow_me":
        message = "Follow Me action executed!"
    else:
        message = "Unknown action."

    # For now, just return a JSON response with the message
    return jsonify({"message": message})

if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True)
