# Oprofile processing scripts

####Scripts for automating profiling percival_processing library using oprofile

## What files are needed?

- calibration_data_generator.py
- output_processing.py
- profile_processing.py

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

- the calibration data is written to *.h5 files where * is the name of the calibration data
- paths to these files are written to test_params_file.txt
- these files are saved to ./oprofile_reports/host_name/, which will be overriden if originally exist.

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
```dls-python ./tools/profile_processing.py```

## Generated files
```
oprof_reports/<host_name>/oprof_data/
oprof_reports/<host_name>/profile_report.txt
[optional] oprof_reports/<host_name>/annotated/
```

## Important variables

