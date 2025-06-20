# Grassroots and the Grassroots Virtual Machine (GRVM)

Grassroots is an attempt to design a full high-level language accompinied with its own assembly language and bytecode that is run using the Grassroots Virtual Machine.

## Building and Running

This project is built using CMake.

- Run `cmake -B build/ -DCMAKE_BUILD_TYPE=DEBUG` to configure the CMake project.
- Run `cmake --build ./build` to build the necessary executables.
- Run the relevant executables from the `build/` directory.

## Planned Features and Road Map

| Version Number | Planned Features                                                                                                                                                                                               |
| -------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0.1.0          | - Grassroots bytecode implementation details.<br>- Grassroots assembly implementation details.<br>- An assembler (the GSM) than can turn Grassroots assembly (`.ga` extension) files <br> into a `.gobj` file. |
| 0.2.0          | - A Grassroots virtual machine (GVM) that can run a given Grassroots bytecode (`.gobj` extension) file.                                                                                                        |
| 0.3.0          | - Grassroots language implementation details.<br>- A compiler that turns Grassroots (`.gr` extension) files into `.ga` files.                                                                                  |

#### Features I'd like to add in, someday

- A simulated OS
- A release version of the project
