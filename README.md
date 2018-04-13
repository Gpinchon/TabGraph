# Table of Contents
- [What is this ?](#what-is-this-)
- [Screenshots](#screenshots)
- [How to use ?](#how-to-use-)
- [Key Binding](#key-binding)
- [Features](#features)
- [System Requirements](#system-requirements)
- [Credits](#credits)

# What is this ?
This is the source code for 42's project scop, a .obj viewer.
See Win64_release for precompiled Windows version

***If you're a student looking for guidance, jump to Credits section ;-)***

# Screenshots
![Artorias](/screenshots/screen.png "Artorias")
![DreamSong](/screenshots/screen1.png "DreamSong")
![Warmonger](/screenshots/screen2.png "Warmonger Sword")
![Cyber Warrior](/screenshots/screen3.png "Cyber Warrior")

# How to use ?
Simply drop a *.obj* file directly onto *Scop*, or run *Scop* through shell with the model's path as first argument as such :
```
./Scop ./some/model/path.obj
```

# Key Binding
- [⇦ ⇨ ⇧ ⇩] Orbit camera around model
- [Keypad -/+] Zoom out/in
- [LCtrl + [Keypad +/-]] Scale up/down model
- [Page Up/Down] Move camera up/down
- [Left Shift] Speed up movements/scaling
- [Space] Cycle through environments
- [LAlt + Enter] Switch fullscreen
- [S] Switch stupidity on/off

# Features
- This program features real time Physically based rendering for *.obj* models.
- It features image based lighting using irradiance maps and a custom BRDF Lookup Table (replaceable).
- It's inspired by Unreal Engine 4 workflow and allows for metallic, roughness and specular (F0).
- The Specular channel on the material allows for more various materials, such as lacquered plastics for instance.
- You can specify an heigth texture to enable steep parallax mapping.
- Here, Specular channels are slightly different from Blinn-Phong's specular channel, it influences the material's reflectivity and behavior regarding light as it is used as a precomputed F0 value. A plastic material with a Specular of *vec3(1, 1, 1)* will have a behavior close to metallic materials, but will have a diffuse channel, unlike metallic material. In order to have "normal" plastic, it is recommended to either leave *Ni* and *Ks* empty, use *Ni 1.5* or *Ks 0.04 0.04 0.04*
- Extra values have been added to mtl files, allowing for physically based materials :
```
Nr [float] //roughness value
Nm [float] //metallic value
Np [float] //parallax factor
map_Nr [./relative/path] //roughness map
map_Nm [./relative/path] //metallic map
map_Nh [./relative/path] //heigth map
map_No [./relative/path] //ambient occlusion map (1 == full occlusion)
```

# System requirements

*Minimum configuration :*
```
CPU : Intel i3 or equivalent
RAM : 1GB
GPU : Intel HD Chipset or equivalent with at least 256Mo VRAM
```

*Recommanded configuration :*
```
CPU : Intel i3 or equivalent
RAM : 1GB
GPU : NVIDIA GPU with at least 256Mo VRAM
```

***May crash on Intel Atom systems***

# Credits
- "Moving Frostbite to PBR" paper by S. Lagarde
- "Physically-Based Shading at Disney" by B. Burley
- Brian Karis blog named GraphicRants
- Unreal Engine's documentation regarding Physically Based Materials
- learnopengl.com for numerous and helpful tutorials
- Khronos documentation
- Apple documentation on GLSL
- Coding Labs for making PBR and BRDF easier to understand
- Wikipedia because we always need an encyclopedia
- Emil Persson and custommapmakers for the free cubemaps
