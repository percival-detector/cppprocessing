import re
import socket
import subprocess
import output_processing as op

def run_application(execute_function, width, height, repeat, max_tokens, max_threads, grain_size):
    #Program to execute
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '
    parallel_debug = './parallel_debug/cppProcessing2.0 '
    parallel_profile = './parallel_profile/cppProcessing2.0 '
    
#     grain_size = 3528
    
    text_file_name = './calibration_data/' + socket.gethostname() + '/test_param_file.txt'
    
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '
    parallel_debug = './parallel_debug/cppProcessing2.0 '
    parallel_profile = './parallel_profile/cppProcessing2.0 '


    result_directory = './oprof_reports/'+ socket.gethostname() + '/'
    report_destination = result_directory + 'bandwidth_report.txt'

    cmdl_arg = '1 '  + str(width) + ' '+ str(height) + ' ' + str(repeat) + ' ' + text_file_name + ' ' + str(grain_size) + ' ' + str(execute_function)   + ' ' + str(max_tokens) + ' ' + str(max_threads)
    program_to_execute = parallel_profile + cmdl_arg

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

def time_function (width, height, repeat, max_tokens, max_threads, grain_size):
    # run_application(0, width, height, repeat)
    time0 = run_application(0, width, height, repeat, max_tokens, max_threads, grain_size)
    time1 = run_application(1, width, height, repeat, max_tokens, max_threads, grain_size)
    
    timediff = (time1 - time0)
    bandwidth = (10 * repeat * width * height * 2 / timediff)
    bandwidth_string = op.get_bytes(bandwidth) + '/s'
    
    return bandwidth
    

width = 3528
height = 3717
repeat = 300
grain_size = 3528
max_tokens = 20
max_threads = 20

bandwidth = time_function(width, height, repeat, max_tokens, max_threads, grain_size)
print op.get_bytes(bandwidth) + '/s'
###############################################################################################################
## Automating varying parameters
# host_name = socket.gethostname()
# calib_directory = './calibration_data/'
# path_name = calib_directory + host_name + '/'
# text_file_name =  path_name + 'test_param_file.txt'
# grain_size_file_name = './oprof_reports/' + host_name + '/' + 'grain_size_test.txt'
# print grain_size_file_name 
# cmd_rm_file = "rm -f " + grain_size_file_name
# cmd_create_file = "touch "+ grain_size_file_name
# op.cmd_call(cmd_rm_file)
# op.cmd_call(cmd_create_file)

# for max_tokens in range(10,50,5):
# #     for grain_size in (3528, 3528/2,3528/3,3528/4,3528/6,3528/7, 3528*3, 3528*7,3528*9, 3528*21, 3528*59, 3528*63, 3528*177,3528*413,3528*531,3528*1239):
#     a = op.accumulator(grain_size_file_name, [max_threads, max_tokens, grain_size], 10)
#     for repeats in xrange(1,10):
#         bandwidth = time_function(width, height, repeat, max_tokens, max_threads, grain_size)
#         a.add_number(bandwidth)
#         print grain_size, op.get_bytes(bandwidth)
#     a.average()

# a = op.accumulator(grain_size_file_name, 1, 10)
# for repeats in xrange(1,10):
#     bandwidth = time_function(width, height, repeat, max_tokens, max_threads, grain_size)
#     a.add_number(bandwidth)
#     print grain_size, op.get_bytes(bandwidth)
# a.average()

         
print bandwidth 
###############################################################################################################







