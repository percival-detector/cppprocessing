cppProcessing
===================================================================

C++ implementation of the Percival processing algorithms

This project is a C++ implementation of pyProcessing algorithm library.

directories
-------------------------------------------------------------------

* data             data from test percival detector chip; data used for BOOST unit test
* Include          header files
* profile          main repeatedly calling algorithm functions, for profiling
* src              .cpp files
* test/unit_test   BOOST unit tests files
* tools            generating HDF5 test data; python script for profiling

List of functions
--------------------------------------------------------------------
```
percival_HDF5_loader
percival_ADC_decode
percival_CDS_correction
percival_ADU-to_electron_correction
percival_HDF5_writer

```
Eclipse Build Configurations
----------------------------

The different products of this project are built using the Eclipse build system.
The following build configurations are available:

#### BOOST_test_ADC_decode

* **Description:**
	Build unit tests
	Require Boost Library and HDF5 C library (dynamic and static) to run
	HDF5 C library can be found in dls-sw
* **Build product:**
	BOOST_test_ADC_decode/cppProcessing2.0
* **Dependencies:**
	source: Include/, src/, tests/,
	test data files: data/
* **Files generated:**
	data/test_write_to_HDF5.hf

#### Profiling

* **Description:**
	Contains a main function. Used for profiling and testing.
	Require HDF5 C library (dynamic and static) to run
	HDF5 C library can be found in dls-sw
* **Build product:**
	Profiling/cppProcessing2.0
* **Dependencies:**
	source: Include/, src/, profile/,
	test data files: data/

#### test_data_generation

* **Description:**
	Generate test data for unit test on HDF5 loader
	Require HDF5 C++ library (dynamic and static) to run. C library will be used in later commits
* **Build product:**
	test_data_generation/cppProcessing2.0
* **Dependencies:**
	source: tools/HDF5_test_file_generator.cpp
* **Files generated:**
	data/test_HDF5.hf
	data/NotAHDF5File.txt

#### Debug

* **Description:**
	Currently unused build configuration
* **Build product:**


#### Release

* **Description:**
	Currently unused build configuration
* **Build product:**



