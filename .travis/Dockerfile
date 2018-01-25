FROM my-base-ubuntu-image:latest
MAINTAINER Sergei Zabolotskikh <sergei@adblockplus.org>

ARG buildtype
ENV BUILDTYPE $buildtype

# https://wiki.debian.org/Multistrap/Environment#debconf
ENV DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true

# Don't forget to pay attention to manually invalidate cache
RUN apt-get update -qqy

#=========================
# Install ssh and git to clone abpcrawler
# as well as wget to get Firefox
#=========================
RUN apt-get install -yqq \
  build-essential \
  git-core \
  python3-pip \
  wget

#================
# Add normal user
#================
ENV NORMAL_USER application
ENV NORMAL_GROUP ${NORMAL_USER}
ENV NORMAL_USER_UID 998
ENV NORMAL_USER_GID 997
RUN groupadd -g ${NORMAL_USER_GID} ${NORMAL_GROUP} \
  && useradd ${NORMAL_USER} --uid ${NORMAL_USER_UID} \
         --shell /bin/bash  --gid ${NORMAL_USER_GID} \
         --create-home

ENV NORMAL_USER_HOME /home/${NORMAL_USER}
ENV PROJECT_HOME /home/${NORMAL_USER}/project

RUN mkdir ${NORMAL_USER_HOME}/.ssh \
  && ssh-keyscan github.com >> ${NORMAL_USER_HOME}/.ssh/known_hosts

RUN chown -R ${NORMAL_USER}:${NORMAL_GROUP} ${NORMAL_USER_HOME}/.ssh \
  && chmod -R 400 ${NORMAL_USER_HOME}/.ssh \
  && chmod 500 ${NORMAL_USER_HOME}/.ssh \
  && chmod 600 ${NORMAL_USER_HOME}/.ssh/known_hosts

RUN mkdir ${PROJECT_HOME}
COPY . ${PROJECT_HOME}
RUN chown -R ${NORMAL_USER}:${NORMAL_GROUP} ${PROJECT_HOME}

USER ${NORMAL_USER}
WORKDIR ${NORMAL_USER_HOME}

RUN curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.33.8/install.sh | bash

RUN bash ${PROJECT_HOME}/.travis/docker-run.sh
