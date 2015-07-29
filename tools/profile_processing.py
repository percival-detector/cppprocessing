
import re
import subprocess
from email._parseaddr import parsedate

def run_the_function(repeat, width, print_result):
    subprocess.call('LD_LIBRARY_PATH="/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-15/prefix/lib"', shell=True)
    subprocess.call('HDF5_DISABLE_VERSION_CHECK="1"', shell=True)
    
#     repeat = 5;
#     width = 1024 * 55
    program_to_execute = './Profiling/cppProcessing2.0 ' + str(width) + ' ' + str(repeat) 
    subprocess.call('(/usr/bin/time -v ' + program_to_execute + ') &> profile_report.txt', shell=True)
    subprocess.call("echo $'\n' >> profile_report.txt", shell=True)
    subprocess.call('operf '+program_to_execute, shell=True)
    subprocess.call('opreport --symbols >> profile_report.txt', shell=True)
    
    
    f = open('./profile_report.txt', 'r')
    s = f.readline()
    total_time = 0
    percentage_time = 0
    while s != '':
        if 'Elapsed (wall clock) time ' in s:
            parsed = s.split(' ')
            time = parsed[len(parsed)-1]
            time = re.split('\:|\.',time)
            
            length = len(time)
            msc = 0
            sec = 0
            min = 0
            hrs = 0
            
            msc = int(time[length-1]) * 10
            if length > 1:
                sec = int(time[length-2]) * 1000
            if length > 2:
                min = int(time[length-3]) * 60 * 1000
            if length > 3:
                hrs = int(time[length-4]) * 60 * 60 * 1000
            total_time = hrs + msc + sec + min
            
        elif 'percival_ADC_decode(percival_frame<short> const&, percival_frame<float>&)' in s:
            delimited = s.split(' ')
            parsed = [item for item in delimited if item != '']
            percentage_time = float(parsed[1])
            
        s = f.readline()
        
    function_time = total_time * percentage_time /100 /repeat
    if print_result == True:
        print '=' * 100
        print 'The function took ' + str(function_time) + 'ms to run.'
        print 'Image size ' + str(width) +' * ' + '160' + ' pixels.' 
        print str(width * 160 * 2) + ' Bytes.'
        print 'Statistics collected for '+str(repeat)+' iterations'
        print '=' * 100
    return function_time

time_array = []
for number in range(0,50,4):
    x = run_the_function(5, 1024 * number, False)
    
    time_array.append(x)
    
for number in range(0,50,4):    
    print number * 1024 * 160 * 2, time_array[number/4]
    
# print time_array

