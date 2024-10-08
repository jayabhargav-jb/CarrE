FROM arm64v8/ros:noetic-ros-base 

RUN mkdir carre && \
    mkdir ros_capstone

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y python3-pip git && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && \
    apt-get install -y ros-noetic-cv-bridge && \
    :
    # rm -rf /var/lib/apt/lists/*

CMD [ "/bin/bash" ]