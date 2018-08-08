Overview
--------
This project consists of four libraries:
* **104Core** - basic data structures, like Image, Point, etc.
* **104Algo** - basic algorithms, like convolution, centered gradient, etc.
* **104IO** - image input-output and some tools for visualization.
* **MSAS** - stands for "MultiScale Analysis of Similarities" and contains related algorithms.

and two executables:
* **StructureTensorApp** - computes Affine Covariant Structure Tensors for a given image.
* **SimilarityMapApp** - computes similarity map for a given point on one image and all points on another image.

Compilation
-----------
Dependencies:
* libpng
* libjpeg
* libtiff

In order to compile everything, create a new folder for build artifacts (e.g. named `build`) and run cmake and make from there:
```
mkdir build
cd build
cmake ..
make
```

Compiled libraries and executables will be placed in the `build/bin/` folder.
 
Usage examples
--------------
*Note: all commands are ussumed to be executed from the `build/bin/` folder.*

To see usage information do 
```
StructureTensorApp --help
```
and
```
SimilarityMapApp --help
```

TODO: add examples