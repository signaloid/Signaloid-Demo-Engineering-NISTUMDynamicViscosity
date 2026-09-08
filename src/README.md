# Source code

## main.c
Template implementation for the main part of a C/C++ demo application.

## kernel.c/h
Dispatches to the mode-specific calculation kernel: `calculateOutputUxHw` (single
distributional draw) or `calculateOutputMonteCarlo` (repeated native Monte Carlo
draws), and provides `dynamicViscositySingleSample`, the per-sample dynamic
viscosity formula shared by both.

## nist-um-dynamic-viscosity-uxhw.c/h
Implements `dynamicViscosityUxHw`, which calculates the distribution of the
dynamic viscosity output using UxHw distributional arithmetic.

## nist-um-dynamic-viscosity-monte-carlo.c/h
Implements `dynamicViscosityMonteCarlo`, which runs the native Monte Carlo loop
over the dynamic viscosity kernel.

## utilities.c/h
These contain utility methods for parsing, setting, and reporting
the usage of demo-specific command-line arguments of C/C++ demo applications.
These methods call similar methods from `common.c` for handling
command-line arguments common to all of our C/C++ demo applications.

## common.c/h
These contain utility methods for parsing, setting, and reporting
the usage of command-line arguments common to all of our C/C++ demo applications,
as well as other methods that we commonly use across our
C/C++ demo applications, e.g., standard methods for I/O handling. These
source files are symlinks to the original files contained in the repository
[Signaloid-Demo-CommonUtilityRoutines](https://github.com/signaloid/Signaloid-Demo-CommonUtilityRoutines)
which is included as a submodule in `submodules/common`.

## uxhw.c/h
These contain methods that implement the probabilistic versions of the methods
in the UxHw API (e.g., `UxHwDoubleGaussDist`) and uses the GNU Scientific Library (GSL)
random number generators to achieve that. This allows building our C/C++ demo applications
natively (i.e., on conventional architectures) and running native Monte Carlo evaluations
of our C/C++ demo applications without modifying the source code.
These source files are symlinks to the original files and are contained in the repository
[Signaloid-Demo-UxHwCompatibilityForNativeExecution](https://github.com/signaloid/Signaloid-Demo-UxHwCompatibilityForNativeExecution)
which is included as a submodule in `submodules/compat`.

## config.mk
Signaloid cores use this file to identify the source codes they will use when
building the C/C++ demo application.

# To Build Natively on Non-Signaloid Platforms

From the repository root, run:
```
make local-build
```
This builds `demo-native-mc` at the repository root, using the source list in
`config.mk` plus the UxHw compatibility shim `uxhw.c`, and links against GSL
(`libgsl` and `libgslcblas` are required). See the
[Prerequisites](../README.md#prerequisites) section in the root README for
installing GSL. Run the resulting binary from the repository root, e.g.,
`./demo-native-mc`.
