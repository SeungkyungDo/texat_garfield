# TeXAT garfield simulation
git@github.com:lilak-project/texat_garfield.git

### how to build
```
mkdir build
cd build
cmake ..
make 
```
The executables will be created in the top source directory.

### files
- DetectorConfiguration.h : All TexAT configuration are in here. Used by other executables

- draw_field : (Executable) Draw field from texat configuration
- texat_gas_simulation : (Executable) Proceed drift simulation.

- ana_output.C : (ROOT macro) Draw electron distribution histograms from output
- make_condor.py : (Python3 macro) For condor submission in KUNPL server

- data : All data files will be written here
- figures : All figures will be written here
