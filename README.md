# gr-torchdsp

Note: this is a GR 4.0 port of https://github.com/gvanhoy/gr-torchdsp

# Install
## Installing TIS Dependencies
Running TIS in a container is the easiest way to run this OOT. To do so, you'll need to have nvidia drivers installed and Docker installed. Once these are installed, you also need the ```nvidia-container-toolkit``` package from apt.

## Installing OOT Dependencies
The following assumes dev-4.0 branch of gnuradio is installed under Ubuntu 22.04 in a prefix `$GR_PREFIX` with a properly sourced environment


### Install TIS client and common libraries
```
cd $GR_PREFIX/src
mkdir tis
cd tis
```

```
git clone --depth 1 https://github.com/triton-inference-server/common.git
git clone --depth 1 https://github.com/triton-inference-server/client.git
cd common && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$GR_PREFIX ../
make -j12
make install
```
```
cd ../../client && mkdir build && cd build
cmake -DTRITON_ENABLE_CC_HTTP=ON -DTRITON_ENABLE_PYTHON_HTTP=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$GR_PREFIX ../ 
make -j12 # make will do the install into the prefix

```

## Installing the OOT
```
meson setup build --buildtype=release --prefix=$GR_PREFIX --libdir=lib
cd build
ninja
ninja install
```

## Running the OOT
```
sudo docker run --gpus all -it -p 8000:8000 --ipc=host --rm -v /path/to/models/directory/in/OOT:/models nvcr.io/nvidia/tritonserver:22.04-py3 tritonserver --log-verbose 0 --model-repository=/models --strict-model-config=false
```
