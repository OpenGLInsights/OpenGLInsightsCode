OpenGL Insights : Hierarchical Depth Culling
KriViewer sources
Project page: http://kri.googlecode.com

Build dependencies and tools:
  * boo-0.9.4 or newer
  * .Net-3.5-compatible framework installed
  * opentk-1.0 sources checked out in the neighbour 'opentk' folder (alternatively, you can grab a binary)
  * Gtk# installed (may require a reboot)
  * SharpDevelop 3.2+ (preferred) or MonoDevelop-boo built from the trunk
  * video driver that support OpenGL 3.2

Open the "tools/viewer/code/viewer.sln" with your IDE and build everything.
If you did everything correctly, it will use the project dependencies to build OpenTK first, then 'ext', 'engine' and 'support' and then, finally, 'viewer'. You can also download OpenTK binaries and setup the dependencies by hand.

If you need help, I'm available through the project tracker:
http://code.google.com/p/kri/issues/list
or directly by email: kvarkus <dog> gmail com

KRI also has an automated error-reporting mechanism. Don't forget to comment on the issue and allow KRI output FTP connection within your firewall.

Good Luck!