#!/bin/sh -e

sudo apt-get update
sudo apt-get install software-properties-common

sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 8611B7A28669BB93

wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'

sudo apt-get update

sudo apt-get install kitware-archive-keyring
sudo apt-key --keyring /etc/apt/trusted.gpg del C1F34CDD40CD72DA

sudo apt-get install -yy -q \
	cmake gcc g++ clang \
	libgl1-mesa-dev \
	qt5-default \
	xxd \
	swig
