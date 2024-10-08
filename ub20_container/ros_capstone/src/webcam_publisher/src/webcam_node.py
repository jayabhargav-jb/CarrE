#!/usr/bin/env python3

import rospy
from sensor_msgs.msg import Image
from cv_bridge import CvBridge
import cv2

def main():
    # Initialize the ROS node
    rospy.init_node('webcam_publisher', anonymous=True)
    
    # Create a publisher for the image topic
    image_pub = rospy.Publisher('/webcam/image_raw', Image, queue_size=10)
    
    # Create a CvBridge object
    bridge = CvBridge()
    
    # Open the webcam
    cap = cv2.VideoCapture(0)  # 0 is usually the default camera
    
    if not cap.isOpened():
        rospy.logerr("Could not open webcam")
        return

    rate = rospy.Rate(10)  # 10 Hz
    while not rospy.is_shutdown():
        ret, frame = cap.read()
        if ret:
            # Convert the frame to a ROS Image message
            ros_image = bridge.cv2_to_imgmsg(frame, encoding="bgr8")
            
            # Publish the image
            image_pub.publish(ros_image)
        
        rate.sleep()

    # Release the webcam
    cap.release()

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass

