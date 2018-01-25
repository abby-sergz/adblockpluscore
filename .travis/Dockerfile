FROM my-base-ubuntu-image:latest
MAINTAINER Sergei Zabolotskikh <sergei@adblockplus.org>


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
  pyhton3-pip
