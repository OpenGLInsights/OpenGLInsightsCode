README

This example shows how to use multiple GPUs for rendering using a simple producer-consumer example.
The producer runs on the secondary GPU generating a simple teapot and then copies the rendered image to the primary GPU that acts as the consumer

Usage
MultiGPU [-verbose] [-streams n] [-runTime s] [-size x y] [-singleGPU]
-verbose              print messages
-streams n            use 'n' streams (default 2). Streams denote the size of the shared buffer that is used. This requires experimentation.
-runTime s            run benchmark for 's' seconds
-size x y			  texture size (default 2048 1024)\n",The texture size is the data transfered across PCIe. Set this to a large number to understand the impact of the bus transfer
-singleGPU				run in singleGPU mode (not default)

To revert to single GPU mode, 