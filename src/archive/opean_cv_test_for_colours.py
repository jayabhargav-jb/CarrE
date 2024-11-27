import cv2
import numpy as np

# Define the HSV range for detecting red
# Red appears in two ranges in HSV space
lower_red1 = np.array([0, 120, 70])
upper_red1 = np.array([10, 255, 255])
lower_red2 = np.array([170, 120, 70])
upper_red2 = np.array([180, 255, 255])

# Initialize the webcam
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

while True:
    # Capture each frame from the webcam
    ret, frame = cap.read()
    if not ret:
        print("Error: Failed to capture image.")
        break

    # Convert the frame to HSV color space
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # Create masks for detecting red color
    mask1 = cv2.inRange(hsv_frame, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv_frame, lower_red2, upper_red2)
    red_mask = mask1 | mask2  # Combine the masks to detect both ranges of red

    # Apply the mask to the original frame to highlight the red areas
    red_highlight = cv2.bitwise_and(frame, frame, mask=red_mask)

    # Display the original frame and the frame with red highlighted
    cv2.imshow("Original Frame", frame)
    cv2.imshow("Red Highlight", red_highlight)

    # Break the loop when 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the webcam and close all OpenCV windows
cap.release()
cv2.destroyAllWindows()
