# Edge Impulse Example: stand-alone inferencing (C++)

This builds and runs an exported impulse locally on your machine. See the documentation at [Deploy your model as a C++ library](https://docs.edgeimpulse.com/docs/deploy-your-model-as-a-c-library). There is also a [C version](https://github.com/edgeimpulse/example-standalone-inferencing-c) of this application.

## Basic steps

 * Download and unzip your Edge Impulse C++ library into this directory
 * Copy a test sample's *raw features* into the `features[]` array in *source/main.cpp*
 * Enter `make -j` in this directory to compile the project
 * Enter `./build/app` to run the application
 * Compare the output predictions to the predictions of the test sample in the Edge Impulse Studio

 ## License

 [Appache License v2.0](https://www.apache.org/licenses/LICENSE-2.0)