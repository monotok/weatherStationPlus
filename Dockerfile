FROM balenalib/raspberry-pi-debian:buster

ARG debuguser=debugger
ARG userid=1000
ARG groupid=1000

RUN apt update

RUN  apt install -y git wget libssl-dev make gcc g++ openssh-server libpq-dev libpqxx-dev build-essential gdb gdbserver rsync vim libconfig++-dev libpcre2-dev \
     && apt-get clean \
     && rm -rf /var/lib/apt/lists/*

WORKDIR /root

# cmake needs to be compiled from source due to it missing a fix in version in repo. Won't link libyang.
# https://gitlab.kitware.com/cmake/cmake/-/issues/20568
RUN git clone https://gitlab.kitware.com/cmake/cmake.git && \
    cd cmake && \
    git checkout v3.21.4 && \
    ./bootstrap
RUN make
RUN make install -j 6 && \
    ln -s /usr/local/bin/cmake /usr/bin/

RUN git clone https://github.com/CESNET/libyang.git && \
    cd libyang && \
    mkdir build && \
    cd build && \
    /usr/local/bin/cmake .. && \
    make -j 6 && \
    make install

RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# 22 for ssh server. 7777 for gdb server.
EXPOSE 22 7777

RUN groupadd -g $groupid $debuguser && useradd -ms /bin/bash -u $userid -g $groupid $debuguser
RUN echo "$debuguser:pwd" | chpasswd

RUN mkdir -p /home/$debuguser/builds/wsp && chown -R $debuguser:$debuguser /home/$debuguser/builds/wsp
WORKDIR /home/$debuguser/builds/wsp
RUN /usr/bin/ssh-keygen -A
CMD ["/usr/sbin/sshd", "-D"]