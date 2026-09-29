# Source Code

## `main.c`
Template implementation for the main part of a C/C++ demo application.
It dispatches to one of two mode-specific kernels: `calculateOutputUxHw`
for a single distributional (Signaloid platform) evaluation, or
`calculateOutputMonteCarlo` for a native Monte Carlo run.

## `kernel.c/h`
The top-level `calculateOutputUxHw` and `calculateOutputMonteCarlo` kernels,
plus the shared `setInputVariables` routine that draws the input variables
(a full distribution on the Signaloid platform, a single sample when running
natively).

## `kernel-uxhw.c/h`
The per-output UxHw® kernels. These operate on distributional `double`s. A
single evaluation propagates the complete input distributions through the
computation. This is where distributional-arithmetic (UxHw API) code lives.

## `kernel-monte-carlo.c/h`
The per-output Monte Carlo kernels. These do not use the UxHw distributional
API. They draw scalar input samples and accumulate one output sample per
iteration into a samples array.

## `utilities.c/h`
These contain utility methods for parsing, setting, and reporting
the usage of demo-specific command-line arguments of C/C++ demo applications.
These methods call similar methods from `common.c` for handling
command-line arguments common to all of our C/C++ demo applications.

## `common.c/h`
These contain utility methods for parsing, setting, and reporting
the usage of command-line arguments common to all of our C/C++ demo applications,
as well as other methods that we commonly use across our
C/C++ demo applications, e.g., standard methods for I/O handling. These
source files are symlinks to the original files contained in the repository
[Signaloid-Demo-CommonUtilityRoutines](https://github.com/signaloid/Signaloid-Demo-CommonUtilityRoutines)
which is included as a submodule in `submodules/common`.

## `uxhw.c/h`
These contain methods that implement the probabilistic versions of the methods
in the UxHw API (e.g., `UxHwDoubleGaussDist`) and uses the GNU Scientific Library (GSL)
random number generators to achieve that. This allows building our C/C++ demo applications
natively (i.e., on conventional architectures) and running native Monte Carlo evaluations
of our C/C++ demo applications without modifying the source code.
These source files are symlinks to the original files and are contained in the repository
[Signaloid-Demo-UxHwCompatibilityForNativeExecution](https://github.com/signaloid/Signaloid-Demo-UxHwCompatibilityForNativeExecution)
which is included as a submodule in `submodules/compat`.

## `config.mk`
Signaloid cores use this file to identify the source codes they will use when
building the C/C++ demo application.

# To Build Natively on Non-Signaloid Platforms

The native build needs a C compiler, GNU Make and the GNU Scientific Library
(GSL). Build from the repository root with the provided `Makefile`:
```
make local-build          # produces ./demo-native-mc
```
It compiles the `config.mk` sources plus `uxhw.c`, the UxHw compatibility
layer, and links GSL.

## macOS

Install the Apple command line developer tools (which provide `make` and the C
compiler), then install GSL with either [Homebrew](https://brew.sh) or
[MacPorts](https://www.macports.org):
```
xcode-select --install
brew install gsl          # or: sudo port install gsl
```

GSL is not on the default compiler search path on macOS. The `Makefile` detects
the install location of MacPorts (`/opt/local`), Homebrew on Apple Silicon
(`/opt/homebrew`) and Homebrew on Intel (`/usr/local`), so no further
configuration is needed. If you are unsure where GSL landed,
`gsl-config --cflags --libs` prints the flags for your installation.

## Linux
```
sudo apt-get install -y build-essential libgsl-dev
make local-build
```
