import threading
import time
from flask import Flask
import socketio
from controller_input.controller import XboxController

# Create a Flask app
app = Flask(__name__)
joy = XboxController()
# Create a Socket.IO server
sio = socketio.Server()
app.wsgi_app = socketio.WSGIApp(sio, app.wsgi_app)

connected_clients = set()

@sio.event
def connect(sid, environ):
    print('Client connected:', sid)
    connected_clients.add(sid)
    # Send initial data to the newly connected client
    data = joy.read()
    sio.emit('cmdStatus', data, to=sid)

@sio.event
def disconnect(sid):
    print('Client disconnected:', sid)
    connected_clients.discard(sid)

def broadcast_data():
    while True:
        if connected_clients:
            data = joy.read()
            # print(data)
            for sid in connected_clients:
                sio.emit('cmdStatus', data, to=sid)
        time.sleep(0.6)  # Adjust the interval as needed

# Start the background thread for broadcasting data
broadcast_thread = threading.Thread(target=broadcast_data)
broadcast_thread.daemon = True
broadcast_thread.start()

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080, debug=True)
