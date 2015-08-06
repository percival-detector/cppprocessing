cppprocessing
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

description of functions
--------------------------------------------------------------------

    |input image data (unsigned int8 HDF5 files)| -> percival_HDF5_loader -> |percival_frame| (raw image)
    |calibration data (32bits float HDF5 files)|  -> percival_HDF5_loader -> |calib_params|
    |percival_frame|   with   |calib_params|      -> percival_ADC_decode  -> |percival_frame| (decoded image)


Eclipse Build Configurations
----------------------------

The different products of this project are built using the Eclipse build system.
The following build configurations are available:

BOOST_test_ADC_decode
^^^^^^^^^^^^^^^^^^^^^

* **Description:**
* **Build product:**

Debug
^^^^^

* **Description:**
* **Build product:**

Profiling
^^^^^^^^^

* **Description:**
* **Build product:**

Release
^^^^^^^

* **Description:**
* **Build product:**

test_data_generation
^^^^^^^^^^^^^^^^^^^^

* **Description:**
* **Build product:**


