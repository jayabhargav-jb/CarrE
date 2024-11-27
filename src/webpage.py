from flask import Flask, render_template, Response, request, jsonify
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

@app.route('/joystick_input', methods=['POST'])
def joystick_input(): 
    try:
        data = request.json
        x = data.get('x', 0)
        y = data.get('y', 0)
        # Process joystick input here
        print(f"Joystick Input - X: {x}, Y: {y}")
        return jsonify({"status": "success", "message": f"Joystick data received: X={x}, Y={y}"})
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000, debug=True)
    # app.run(debug=True)
