# ICAST

Icast is a DDS (Data Distribution Service) implementaion that use UDP Multicast with TDMA syncronization.
Icast use a simple key-value API that can be managed through dictionary.json

## Install

```
git clone https://github.com/hernanda16/ICAST.git
mkdir build
cd build
cmake ..
source ~/.bashrc
make
sudo make install
```

## Usage

There are some tips for use this icast

### Dictionary

You can always change your data dictionary on dictionary.json. Remember, always do `cmake .. ` if you've changed the dictionary.json

### Configs

You can change your own config of icast.cfg

## Run

You can run a simple demo program in ./example/

```
cd $ICAST_DIR/build
./simple_icast
```

## Note

Use `cmake -DUSE_FIFO=ON ..` to compile it using FIFO scheduler
