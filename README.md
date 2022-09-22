Spindulys
======

## Introduction

A simple and light weight ray tracer.

You want to quickly want to trace a scene then this project is for you!

## Installation

At this current moment in time you can only build it from source, done by,

```
git clone git@github.com:LinasBeres/spindulys.git
cd spindulys
git submodule update --init --recursive
```

and then build,

```
mkdir build
cd build
cmake ..
make
```

### Requirements

These are the current packages needed to build Spindulys:
- clang/gcc (clang-7+/gcc-8+)
- cmake (3.13+)
- [Intel Embree](https://github.com/embree/embree) (3.5.2)
- [USD](https://github.com/PixarAnimationStudios/USD) (20.11 - 22.08)
- [TBB](https://github.com/oneapi-src/oneTBB) (2018.U6+)

In the future these dependancies should either be minimised or completely removed.

### Running
Currently only USD scenes are supported. Launch one by,
```
spindulys -s <path/to/usdScene>
```

Then move the camera around with the WASD keys and right mouse button to rotate.

## Acknowledgements
This was orignally forked from https://github.com/JoshuaSenouf/tracer into https://github.com/LinasBeres/tracer, but then as I made more changes I decided to turn it into it's own repository.

---

## TODO List
| TODO                  | Done | Extras                                                                                                     |
| ---------             | ---- | ------                                                                                                     |
| USDCamera             | [x]  | <ul><li>depth of field []</li><li>near/far []</li><li>clipping planes []</li><li>orthographic []</li></ul> |
| USDMesh               | [/]  |                                                                                                            |
| USDCube               | []   |                                                                                                            |
| USDCylindar           | []   |                                                                                                            |
| USDSphere             | []   |                                                                                                            |
| USDCone               | []   |                                                                                                            |
| USDBasisCurves        | [/]  |                                                                                                            |
| USDPoints             | []   |                                                                                                            |
| USDNurbsCurves        | []   |                                                                                                            |
| USDNerbsPatch         | []   |                                                                                                            |
| USDCapsule            | []   |                                                                                                            |
| USDInstance           | []   |                                                                                                            |
| XML Loader            | []   |                                                                                                            |
| Remove USD Dependency | []   |                                                                                                            |
| Support Mac           | []   |                                                                                                            |
| Support Windows       | []   |                                                                                                            |
