import re
import socket
import subprocess
import output_processing as op

def run_application(execute_function, width, height, repeat):
    #Program to execute
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '
    parallel_debug = './parallel_debug/cppProcessing2.0 '
    parallel_profile = './parallel_profile/cppProcessing2.0 '
    
    grain_size = 3528
    
    text_file_name = './calibration_data/' + socket.gethostname() + '/test_param_file.txt'
    
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '
    parallel_debug = './parallel_debug/cppProcessing2.0 '
    parallel_profile = './parallel_profile/cppProcessing2.0 '


    result_directory = './oprof_reports/'+ socket.gethostname() + '/'
    report_destination = result_directory + 'bandwidth_report.txt'

    cmdl_arg = '1 '  + str(width) + ' '+ str(height) + ' ' + str(repeat) + ' ' + text_file_name + ' ' + str(grain_size) + ' ' + str(execute_function)   
    program_to_execute = parallel_debug + cmdl_arg

    cmd_time = '(/usr/bin/time -v ' + program_to_execute + ') &> ' + report_destination
    op.cmd_call('mkdir -p ' + result_directory)
    op.cmd_call(cmd_time)

    time = parse(report_destination)
    print cmd_time
    return time

def parse(file):
    f = open(file, 'r')
    s = f.readline()
    total_time = 0
    while s != '':
        if 'Elapsed (wall clock) time' in s:
            total_time = op.parse_time(s)
        s = f.readline()
    f.close()
    print total_time/1000
    return total_time/1000

width = 3528
height = 3717
repeat = 100
# run_application(0, width, height, repeat)
time0 = run_application(0, width, height, repeat)
time1 = run_application(1, width, height, repeat)

timediff = (time1 - time0)
bandwidth = op.get_bytes(10 * repeat * width * height * 2 / timediff) + '/s'

print bandwidth 
