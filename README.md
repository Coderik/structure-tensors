This project consists of four libraries:
* 104Core
* 104Algo
* 104IO
* MSAS

and two executables:
* StructureTensorApp
* SimilarityMapApp

In order to compile everything create a new folder for build artifacts (e.g. named `build`) and run cmake and make from there:
```
mkdir build
cd build
cmake ..
make
```

Compiled libraries and executables will be placed in the `build/bin/` folder.
 
To see usage information do 
```
StructureTensorApp --help
```
and
```
SimilarityMapApp --help
```