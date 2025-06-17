# Ubuntu 18.04
FROM ubuntu:bionic

# Tools I find useful during development
RUN apt-get update \
 && apt-get install -y \
        cmake \
        pkg-config \
        cppcheck \
        git \
        mercurial \
        build-essential \
        curl \
        libprotobuf-dev \
        protobuf-compiler \
        libprotoc-dev \
        libzmq3-dev \
        net-tools \
        uuid-dev \
        doxygen \
        ruby-ronn \
        libsqlite3-dev \
        g++-8 \
        sudo \
        gnupg \
        lsb-release \
        wget \
 && apt-get clean

# Use GCC 8
 RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 \
        --slave /usr/bin/g++ g++ /usr/bin/g++-8 \
        --slave /usr/bin/gcov gcov /usr/bin/gcov-8

# Set USER and GROUP
ARG USER=developer
ARG GROUP=developer

# Add a user with the same user_id as the user outside the container
# Requires a docker build argument `user_id`.

RUN curl -SsL https://github.com/boxboat/fixuid/releases/download/v0.4/fixuid-0.4-linux-amd64.tar.gz | tar -C /usr/local/bin -xzf - && \
    chown root:root /usr/local/bin/fixuid && \
    chmod 4755 /usr/local/bin/fixuid && \
    mkdir -p /etc/fixuid && \
    printf "user: $USER\ngroup: $GROUP\n" > /etc/fixuid/config.yml

RUN addgroup --gid 1000 $USER && \
    adduser --uid 1000 --ingroup $USER --home /home/$USER --shell /bin/sh --disabled-password --gecos "" $USER

RUN adduser $USER sudo \
 && echo "$USER ALL=NOPASSWD: ALL" >> /etc/sudoers.d/$USER

# Commands below run as the developer user.
USER $USER:$GROUP

# When running a container start in the developer's home folder.
WORKDIR /home/$USER

RUN export DEBIAN_FRONTEND=noninteractive \
 && sudo apt-get update \
 && sudo -E apt-get install -y \
    tzdata \
 && sudo ln -fs /usr/share/zoneinfo/America/Los_Angeles /etc/localtime \
 && sudo dpkg-reconfigure --frontend noninteractive tzdata \
 && sudo apt-get clean

# Install Ignition dependencies
RUN sudo /bin/sh -c 'echo "deb [trusted=yes] http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list' \
 && sudo /bin/sh -c 'wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -' \
 && sudo apt-get update \
 && sudo apt-get install -y \
    libignition-cmake2-dev \
    libignition-math6-dev \
    libignition-msgs8-dev \
    libignition-utils1-cli-dev \
 && sudo apt-get clean

# Ignition transport
RUN git clone https://github.com/ignitionrobotics/ign-transport.git \
 && cd ign-transport \
 && mkdir build \
 && cd build \
 && cmake .. -DCMAKE_INSTALL_PREFIX=/usr \
 && sudo make -j4 install \
 && cd ../..

# Ignition transport examples
RUN cd ign-transport/example \
 && mkdir build \
 && cd build \
 && cmake .. \
 && make -j4 \
 && cd ../..

# Customize your image here.
# E.g.:
# ENV PATH="/opt/sublime_text:$PATH"
