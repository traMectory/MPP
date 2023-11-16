# MPP

To build:
```
<mkdir build>
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

To run (from `build` folder):
```
src/mpp [path_to_input_file] [path_to_output_folder] <-v to visualize> <-a 0|1|2 to choose an algorithm>
