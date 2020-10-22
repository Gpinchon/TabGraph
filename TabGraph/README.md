# TabGraph
*tabGraph* is a full SceneGraph featuring a flexible Component system, giving a lot of freedome for adding new components to your scene and organizing it.

# Component system
This SceneGraph revolves around a Component system, to be short : everything is a Component (or will be)
There are two reasons to this :
 - It lets the user organise his Scene Graph as he wishes (although there are some predefined schemes).
 - Updates and Fixed Updates are automatic and handled gracefully.
 
 # Updates
 Components feature several update types :
  - UpdateCPU : An update that's run on every loop on the engine on the host. Always run before FixedUpdate. Use this for cheap mandatory updates.
  - FixedUpdateCPU : An update that's run once every 0.016 seconds, can be farther appart if the engine's loop takes more time for some reason. Use this for expensive updates.
  - UpdateGPU : An update that's run everytime the Renderer draws a frame.
  - FixedUpdateGPU : Same as FixedUpdateCPU, except it's designed for updates regarding the GPU.
 
 # Threads
 This SceneGraph is multithreaded :
  - The first thread handles the Engine and the various CPU Updates. Won't run updates if the renderer is working.
  - The second thread handles the rendering and GPU updates. Won't run updates if rendering is not needed.
 
