[![PyPI version](https://badge.fury.io/py/pyqryptonight.svg)](https://badge.fury.io/py/pyqryptonight)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4b34f51616d94362b3447bb2f4df765a)](https://www.codacy.com/app/jleni/qryptonight_QRL?utm_source=github.com&utm_medium=referral&utm_content=theQRL/qryptonight&utm_campaign=badger)
[![Build Status](https://travis-ci.org/theQRL/qryptonight.svg?branch=master)](https://travis-ci.org/theQRL/qryptonight)
[![GPL3 licensed](https://img.shields.io/badge/license-GPL3-blue.svg)](https://raw.githubusercontent.com/theQRL/qryptonight/master/LICENSE)

# Qryptonight

*WARNING: This is work in progress, changes might not be backward compatible.*

This library (derived from xmr-stak) currently exposes the following functionality:  

- Python Wrapper for Cryptonight hash
- Mining thread and hybrid callback 

**Platform support**

|           | Linux |     OSX<br>10.12     |  Windows<br>10 | Raspbian<br>? | 
|-----------|:------------:|:-----------:|:--------:|:--------:|
|Python 3   | :white_check_mark: | :white_check_mark: |    :seedling:     |     :question:    |
|Webassembly (JS) |      :question:       |     :question:       |    :question:     |     :question:    |
|Golang     | - |     -       |    -     |     -    |

## Installing

#### Ubuntu
```
sudo apt -y install swig3.0 python3-dev build-essential cmake libhwloc-dev libboost-dev
pip3 install pyqryptonight
````

#### OSX

If you dont have brew yet, we think you should :) Install brew following the instructions here: [https://brew.sh/](https://brew.sh/)

Now install some dependencies

```bash
brew install cmake python3 swig hwloc boost
pip3 install pyqryptonight
```

#### Windows
```
TBD
```

## License

*This library is distributed under the GPL3 software license

Some of the code is based on the xmr-stak implementation. Most of third party code has been included as git submodules for future reference.
