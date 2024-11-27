import cv2

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Camera not accessible")
else:
    print("Camera accessible")
cap.release()
