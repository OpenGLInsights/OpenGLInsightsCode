___________
README.txt

Works for large data sizes
transfer time == render time

Default set to do both render, upload and readback.
maxIterations  = 100 => 10ms render time on a quadro 5000


_____
NOTES
Performance is very sensitive to the motherboard chipset as well as SBIOS
eg
- hp z800 readback is 1/2 that of upload? confirmed with cudaBandwidth test
bandwidthTest.exe --memory=pinned
 Device 0: Quadro 6000
 Quick Mode

 Host to Device Bandwidth, 1 Device(s), Pinned memory
   Transfer Size (Bytes)        Bandwidth(MB/s)
   33554432                     5757.7

 Device to Host Bandwidth, 1 Device(s), Pinned memory
   Transfer Size (Bytes)        Bandwidth(MB/s)
   33554432                     3415.8

 Device to Device Bandwidth, 1 Device(s)
   Transfer Size (Bytes)        Bandwidth(MB/s)
   33554432                     112930.5
   
-z800

_____
TODO
- add upload and readback from a separate thread to avoid memcpy in the same thread, check perf
- add rendering to screen for validation purposed
- add some calibration time before the real benchmark time
- automated calibration so that the app converges to the optimal rendering settings so have transfer time = render time
- glContext - update to ogl 3.2 wglCreateContextAttribsARB isntead of using wglShareLists, need a dummy context first for ARB_DEBUG_OUTPUT
- For all stages + glContext, check all the OGL extensions are available
