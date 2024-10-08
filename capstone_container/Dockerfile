FROM capstone:l4t-r32.7.1

RUN mkdir carre && \
    mkdir ros_capstone

# RUN sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'

COPY ./requirements.txt /requirements.txt

VOLUME [ "/carre" ]=

RUN apt-get update && \
    apt-get upgrade -y && \
    # apt-get install -y ros-noetic-cv-bridge && \
    :
    # rm -rf /var/lib/apt/lists/*

# RUN apt-get update && \
#     apt-get install -y ros-noetic-cv-bridge

RUN apt-get update && \
    apt install software-properties-common && \
    add-apt-repository ppa:deadsnakes/ppa && \
    :

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
    python3-pip git && \
    # python3.7 python3.7-pip git && \
    :
    # rm -rf /var/lib/apt/lists/*

# RUN apt-get install ros-noetic-cv-bridge ros-noetic-image-transport

# RUN python3 -m pip install -r requirements.txt
# RUN python3 -m venv carre && \
#     . /carre/bin/activate

WORKDIR /carre

CMD [ "/bin/bash" ]
