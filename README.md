# Edge Impulse Example: stand-alone inferencing (C++)

This builds and runs an exported impulse locally on your machine. See the documentation at [Deploy your model as a C++ library](https://docs.edgeimpulse.com/docs/deployment/running-your-impulse-locally/running-your-impulse-locally). There is also a [C version](https://github.com/edgeimpulse/example-standalone-inferencing-c) of this application.

> **No hardware optimization!** This example repository contains the bare minimum example of compiling your impulse, and does not contain hardware optimizations for MPUs or on Linux. See [example-standalone-inferencing-linux](https://github.com/edgeimpulse/example-standalone-inferencing-linux) to compile with optimizations.

## Basic steps

 * Download and unzip your Edge Impulse C++ library into this directory
 * Copy a test sample's *raw features* into the `features[]` array in *source/main.cpp*
 * Enter `make -j` in this directory to compile the project. If you encounter any OOM memory error try `make -j4` (replace 4 with the number of cores available)
 * Enter `./build/app` to run the application
 * Compare the output predictions to the predictions of the test sample in the Edge Impulse Studio

 ## License

 [Apache License v2.0](https://www.apache.org/licenses/LICENSE-2.0)
