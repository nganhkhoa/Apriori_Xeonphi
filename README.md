# Apriori Association Rules

Assignment Parallel Computing

## Description:

Association rules on Xeon-phi using OpenMP.

## Students:

- 1610179 - Nguyen Le Chi Bao
- 1612373 - Le Van Nhat
- 1610132 - Dang Hoang An
- 1611617 - Nguyen Anh Khoa

## Build steps

```
git clone --recurse-submodules https://github.com/nganhkhoa/Apriori_Xeonphi.git
```

First build libcsv, a module to read csv files in C.
```
cd libcsv/
autoreconf -f -i
./configure
make
```

Create a build folder to run cmake.
```
mkdir -p build
cd build
cmake ..
make
```

The binary is locate out side, run the binary with one argument as the csv file.
```
./r
```

To build with debug, `cmake .. -DCMAKE_BUILD_TYPE=DEBUG`, the output file is `apriori_debug`. To write code run in debug only, write code inside `#ifdef` gaurd.

```
#ifdef DEBUGGING
// code run only in debug
#endif
```

References:
- [Algorithm](https://github.com/zHaytam/AprioriAlgorithm)
- [Titanic Dataset](https://raw.githubusercontent.com/zHaytam/AprioriAlgorithm/master/titanic_train.csv)

Library used:
- OpenMP
- [libcsv](https://github.com/rgamble/libcsv)
