# Black hole simulator

Generated OpenGL starter project using GLFW, GLAD, GLM, and Dear ImGui.

## Build

```bash
cmake -S . -B build
cmake --build build
```

Or use the provided helper script:

```bash
./build.sh [Debug|Release|RelWithDebInfo|MinSizeRel] [-r|--run] [-fmt|--format]
```

Flags (all optional):

- `Debug|Release|RelWithDebInfo|MinSizeRel` — choose the CMake build type (default: `Debug`)
- `-r`, `--run` — run the built binary after a successful build
- `-fmt`, `--format` — run `./scripts/format-all.sh` before configuring (requires that script)

## Run

```bash
./build/black_hole_simulator
```
