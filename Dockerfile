# 编译镜像命令 docker build --no-cache -t centos:v1 .
# 运行镜像 docker run -itd -p 2222:22 -p 8080:8080 centos:v1 /bin/bash
# 进行centos镜像映射
FROM centos

MAINTAINER lzerone

RUN mkdir -p /usr/develop
WORKDIR /usr/develop

# 因为centos8 停止服务 所以需要更换镜像源
# https://blog.csdn.net/qq_575775600/article/details/125274121
RUN cd /etc/yum.repos.d/ && \
    sed -i 's/mirrorlist/#mirrorlist/g' /etc/yum.repos.d/CentOS-* && \
    sed -i 's|#baseurl=http://mirror.centos.org|baseurl=http://vault.centos.org|g' /etc/yum.repos.d/CentOS-* && \
    yum -y install wget && \
    wget -O /etc/yum.repos.d/CentOS-Base.repo https://mirrors.aliyun.com/repo/Centos-vault-8.5.2111.repo && \
    yum clean all && \
    yum makecache

# sshd环境配置
# https://blog.csdn.net/hyunbar/article/details/122512140
# 安装sshd
RUN yum install -y openssh-server openssh-clients && \
    sed -i '/^HostKey/'d /etc/ssh/sshd_config && \
    echo 'HostKey /etc/ssh/ssh_host_rsa_key'>> /etc/ssh/sshd_config
 
# 生成 ssh-key
RUN ssh-keygen -t rsa -b 2048 -f /etc/ssh/ssh_host_rsa_key
 
# 更改 root 用户登录密码为
RUN echo 'root:root' | chpasswd
 
# 开发 22 端口
EXPOSE 22
 
# 镜像运行时启动sshd
RUN mkdir -p /opt && \
    echo '#!/bin/bash' >> /opt/sshd_start.sh
RUN echo '/usr/sbin/sshd -D' >> /opt/sshd_start.sh
RUN chmod +x /opt/sshd_start.sh
CMD ["/opt/sshd_start.sh"]

# clang开发环境
RUN yum install -y clang

# cmake开发环境
RUN yum install -y cmake

# git环境
RUN yum install -y git

# golang环境也需要进行部署
RUN yum install -y go
RUN go env -w GO111MODULE=on && \
    go env -w GOPROXY=https://goproxy.cn,direct && \
    go env -w CGO_ENABLED=0 && \
    go env

# Jenkins ci/cd工具
# 这里往github拉取最新代码 然后进行检查 出包
RUN wget -O /etc/yum.repos.d/jenkins.repo https://pkg.jenkins.io/redhat-stable/jenkins.repo
RUN rpm --import https://pkg.jenkins.io/redhat-stable/jenkins.io.key

RUN yum install -y epel-release
RUN yum install -y java-1.8.0-openjdk-devel.x86_64
RUN yum install -y jenkins

# 开发 8080 端口 jenkins默认端口
EXPOSE 8080

# 镜像运行时启动jenkins
RUN mkdir -p /opt && \
    echo '#!/bin/bash' >> /opt/jenkins_start.sh
RUN echo '/usr/bin/jenkins -D' >> /opt/jenkins_start.sh
RUN chmod +x /opt/jenkins_start.sh
CMD ["/opt/jenkins_start.sh"]
