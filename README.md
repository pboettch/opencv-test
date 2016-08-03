# Build locally on a PC

Create a build-dir somewhere and get inside

    mkdir build
    cd build

Run cmake and build it

    cmake <path-to-the-source>
    make

In case you're missing opencv, in Debian the package

    apt install libopencv-dev

has to be installed.

To run the example the environment-variable OPENCV\_SHARE needs to be set:

    OPENCV_SHARE=/usr/share/opencv ./webcam-opencv

or

    OPENCV_SHARE=/usr/share/opencv ./webcam-v4l2

whereas /usr/share/opencv is the path on Debian. Depending on your platform it my well be somewhere else.

# Build within Alchemy

Just put this repo into packages and run

    ./build.sh -A <target-name> final -p dragonfly-x1

