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

Draw elliptical patched on a set of points specified in the text file 'points.txt':
```
StructureTensorApp image.png -m ellipses --points points.txt
```

Set color of elliptical patches:
```
StructureTensorApp image.png -m ellipses --hue 120
```

Compute structure tensors using given parameters (number of iterations = 35, radius = 150) and write the result to the file with 'result' prefix:
```
StructureTensorApp image.png -i 35 -r 150 -o result
```

Compute average size of elliptical patches on a set of points regularly distributed over the image every 25 pixels:
```
StructureTensorApp image.png -m avg_size -s 25
```

Compute similarity map between the point (100, 25) on 'image.png' and whole 'image2.png':
```
SimilarityMapApp image.png image2.png -p 100:25
```

Compute similarity map between the point (220, 310) and all other points of the image 'image.png' and write the result to the file with 'result' prefix:
```
SimilarityMapApp image.png -p 220:310 -o result
```

Compute similarity map as before but output raw distances in a text file:
```
SimilarityMapApp image.png -p 220:310 -o result --raw
```

Compute similarity map as before specifying scale, radius and gamma parameters:
```
SimilarityMapApp image.png -p 220:310 -t 0.1 -r 150 -v 1.0 -g 0.6
```