# Westy

Westy is a Vulkan-based rendering engine written in C.  I hope you enjoy my work.  :)
<br>
<br>
<img src="https://github.com/kadealicious/Westy-Vulkan/blob/main/project/docs/screenshots/viking%20home.png" style="width:50%; margin:auto auto -0.25em -0.25em;">

<br>

## Feature Set

### Pipeline
- Dynamic state viewport for faster swap chain recreation.
- Rasterization-based pipeline, raytraced pipeline in-progress.
- Sample rate shading support to mitigate any aliasing caused by fragment shader artifacting.
- MSAA support for up to 64 samples.

### Geometry
- Embedded .gltf mesh loading.

### Texturing
- Mipmapping & dynamic mip LOD control.
- .png, .jpg, .bmp, .tga, & .hdr image format support.

<br>

## Created by Kade Samson
- Linkedin:		[@kadesamson](https://www.linkedin.com/in/kadesamson/)
- Github:		[@kadealicious](https://github.com/kadealicious)
- Instagram:	[@kadesamson](https://www.instagram.com/kadesamson/)
- Portfolio:	[kadesamson.com](https://www.kadesamson.com)
- Everything else: [linktr.ee/kadealicious](https://linktr.ee/kadealicious)

## Libraries Used
- [Vulkan](https://www.vulkan.org/)
- [GLFW](https://www.glfw.org/)
- [cgltf](https://github.com/jkuhlmann/cgltf)
- [stb_image.h](https://github.com/nothings/stb/tree/master)

## Machines Tested
- i9-13900H, RTX 4070 8gb Laptop, 48gb RAM
- i5-7500, RTX 3070 8gb, 16gb RAM
- i5-7500, RX 470 4gb, 16gb RAM
- i5-1035G1, Intel UHD Graphics 128mb, 8gb RAM
