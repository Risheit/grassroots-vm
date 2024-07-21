# Grassroots and the Grassroots Virtual Machine (GRVM)

Grassroots is an attempt to design a full high-level language accompinied with its own assembly language and bytecode that is run using the Grassroots Virtual Machine.

## Building and Running

Build the GRVM by calling `make` while in the root directory. The compiled virtual machine executable `vm.exe` can be found and run from the `build/` directory.

> [!NOTE]
> Currently, custom make options are limited. A release build step and more `make` options are planned.

## Planned Features and Road Map
| Version Number  | Planned Features |
|---|---|
| 0.1.0 | - Grassroots bytecode implementation details<br>- GRVM that can run a given Grassroots bytecode (`.gobj` extension) file. |
| 0.2.0 | - Grassroots assembly implementation details<br>- An assembler than can turn Grassroots assembly (`.ga` extension) files <br>  into a `.gobj` file. |
| 0.3.0 | - Grassroots language implementation details<br>- A compiler that turns Grassroots (`.gr` extension) files into `.ga` files. |

#### Features I'd like to add in, someday
- A simulated OS
- A release version of the project
