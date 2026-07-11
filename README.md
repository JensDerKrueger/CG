<p align="center">
  <img src="mantis-shrimp-logo.png" width="180" alt="Computer Graphics and Visualization Group logo">
</p>

# Computer Graphics Bachelor Course Exercises

This repository contains the programming exercises for the Computer Graphics
Bachelor Course taught by the Computer Graphics and Visualization Group at the
Computer Science Department of the University of Duisburg-Essen.

The exercises introduce central topics from the course, including mesh loading,
geometric intersections, splines, color spaces, rasterization, interpolation,
local illumination, Phong shading, bump mapping, ray casting, recursive ray
tracing, texture coordinates, texture sampling, OpenGL, transformations, and
spatial data structures. Most folders contain a self-contained exercise project
together with native, WebAssembly, Visual Studio, and Xcode build support where
applicable.

## Repository Structure

- Numbered exercise directories contain the individual exercise projects
- `Utils`: shared teaching framework used by the exercises
- `VS`: Visual Studio workspace files
- `CG.xcworkspace`: Xcode workspace
- `makefile`: command-line build entry point for native and WebAssembly builds
- `ShaderPlayground`: small helper project for experimenting with shaders

Each exercise directory is intended to be understandable on its own, while the
shared code in `Utils` keeps window creation, OpenGL setup, image handling,
camera interaction, text rendering, and platform differences out of the core
exercise code.

## Exercises

The numbered exercise directories form the practical part of the course. They
are ordered roughly along the lecture progression, starting with basic geometry
and image handling and moving toward rasterization, shading, ray-based
rendering, OpenGL, and spatial data structures.

Each exercise folder contains the files, assets, build projects, and assignment
material needed for that exercise.

## Building

The repository contains several build setups because the exercises are used in
different teaching environments:

- Makefiles for native and WebAssembly builds
- Visual Studio project files for Windows
- Xcode project files for macOS

Individual exercise folders contain the project-specific files and assets
needed for that exercise. The shared `Utils` folder contains additional
documentation for the small framework used throughout the course.

## License

Copyright (c) 2026 Computer Graphics and Visualization Group, University of
Duisburg-Essen

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files, to deal in the Software
without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
