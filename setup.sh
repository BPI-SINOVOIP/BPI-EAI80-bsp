
sudo apt-get install --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-tree-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
  make gcc libsdl2-dev
#  make gcc gcc-multilib g++-multilib libsdl2-dev
pip3 install pyelftools
pip3 install pyyaml

#
## cmake -DBOARD=banpiboard_qfn80pin ..
#