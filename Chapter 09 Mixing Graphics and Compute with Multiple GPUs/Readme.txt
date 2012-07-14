Interoperability Sample
-----------------------------------------------------------------------------------------
This sample is designed to test CUDA-OpenGL interoperability performance in a system.
- Requires CUDA version 5.0 or greater.
- It creates a GLUT window and tests mapping/unmapping of a texture of a given size
  using GL/CUDA interoperability on every CUDA device available in the system.
- CUDA is the producer and OpenGL is the consumer in this benchmark. This model is implemented by having
  the interoperability mapping flags set to cudaGraphicsMapFlagsWriteDiscard.
- The application computes "speedup" for each CUDA/GL context configuration possible in a given system.
  The speedup is the frame rate compared to pure workload(no interoperability) framerate 
- The application also auto-adjusts the time it spends in GL work and CUDA kernels
  so that it simulates a specified workload (default is 60 FPS).  This is because
  the number of iterations is sensitive to compiler changes, clocks, GPU type, etc.
-----------------------------------------------------------------------------------------
Usage
-----------------------------------------------------------------------------------------

> Interop.exe [-width #] [-height #] [-targetMsecCUDA #] [-targetMsecGL #]

-Program parameters:
	-width #       		- width of the interop texture. Default 1920
	-height #      		- height of the interop texture. Default 1080
	-targetMsecCUDA #	- The time to spend doing CUDA work in msec(can be fractions). Default 17msec
	-targetMsecGL #		- The time to spend doing OpenGL work in msec(can be fractions). Default 17msec			

-Sample Output:

OpenGL is using CUDA device(s): 1
  Tuning knobs to make GL section take ~10.000 msec... 4.88 with 65536 iterations...9.87 with 134158 iterations...

CUDA is using device 0
CUDA and OpenGL contexts reside on different devices
  Benchmarking using a 4096x4096 texture:
  Tuning knobs to make CUDA section take ~10.00 msec... 4.80 with 65536 iterations...9.99 with 136435 iterations...
  Benchmarking CUDA part of the program
    9.999358 msec/frame
  Benchmarking GL part of the program
    10.004820 msec/frame
  Benchmarking CUDA and GL combined, using interop
    34.617699 msec/frame
    Speedup(versus pure workload): 0.58x
    Overhead: 24.61 msec/frame
    Overhead: 71.10%

CUDA is using device 1
CUDA and OpenGL contexts share the same device
  Benchmarking using a 4096x4096 texture:
  Tuning knobs to make CUDA section take ~10.00 msec... 10.01 with 136524 iterations...10.01 with 136412 iterations...
  Benchmarking CUDA part of the program
    10.001414 msec/frame
  Benchmarking GL part of the program
    10.007576 msec/frame
  Benchmarking CUDA and GL combined, using interop
    21.230110 msec/frame
    Speedup(versus pure workload): 0.94x
    Overhead: 1.22 msec/frame
    Overhead: 5.75%

