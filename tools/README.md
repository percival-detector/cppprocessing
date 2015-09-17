# Oprofile processing scripts

####Scripts for automating profiling percival_processing library using oprofile

## What files are needed?

- calibration_data_generator.py
- output_processing.py
- profile_processing.py
- bandwidth.py

## What are the assummed located of files
- generated calibration data will be in ```calibration_data/host_name/```
- OProfile directory will be in ```oprof_report/host_name/```
- annotated source will be in ```oprof_report/host_name/```
- bandwidth.py output will be in ```oprof_report/host_name/```
- profile results will be displayed and the raw profile result will also be written to ```oprof_report/host_name/```.

If any of the following script does not work especially when writing to file, it is probably because some folders do not exist. If they return such errors, touch the file before running the script.

## What do they do?

### calibration_data_generator.py
- it randomly generates the following calibration data: 

```
	Gc, Oc, Gf, Of, 
	Gain_lookup_table1
	Gain_lookup_table2
	Gain_lookup_table3
	Gain_lookup_table4
	ADU_to_electron_conversion
```

- the calibration data is written to '*.h5' files where '*' is the name of the calibration data
- paths to these files are written to test_params_file.txt
- these files are saved to ./oprofile_reports/host_name/, which will be overriden if originally exist.
- range of these randomly generated values can be set.
- takes a while if the arrays are large.

### output_processing.py
- mainly for parsing and post-processing.
- a class inition, which represent events being counted. ```class oprofile_events```

- functions are:


Function | Description
---------|-------------
get_operf_option(list) | combine all events intended to be counted and return a string in cmdline option format
get_llc_misses(list_of_functions, attributes, list_of_events_recorded) | 
get_CPI(list_of_functions, attributes, list_of_events_recorded) |
get_bandwidth(list_of_functions, attributes, total_time, image_size, repeat) |
get_time(list_of_functions, attributes, total_time, repeat) | calculates function time per repeat in seconds from total time and percentage time.
get_bytes(bytes) | converting bytes of kB, MB, and GB, returns a string.
get_function_list(file) | scans the oprofile output file to gather a list of recorded functions, which is a subset of list of functions specified inside the function.
parse_time(time_string) | input time format is that of usr/bin/time in mm:ss.cs (e.g. 01:35.89), return time in milliseconds.


### profile_processing.py
- function body that runs the entire groups of functions.

## How to run this
cd to the top-level directory, which contains tools/ directory.
```dls-python tools/profile_processing.py```

## Generated files
```
oprof_reports/<host_name>/oprof_data/
oprof_reports/<host_name>/profile_report.txt
[optional] oprof_reports/<host_name>/annotated/
```
### bandwidth.py
Runs the test program with the function on and off. Then it takes the difference in time to compute the bandwidth of the processsing algorithm.
If needed, ```use the accummulator``` object in output_processing.py to calculate average and standard deviation, and write the result to a text file. All graphs in my report are gotten using this script
Run with
```dls-python tools/bandwidth.py```

