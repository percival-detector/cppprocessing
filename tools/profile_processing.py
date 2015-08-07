
import re
import subprocess

def run_the_function(print_result):
    #subprocess.call('LD_LIBRARY_PATH="/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-15/prefix/lib"', shell=True)
    #subprocess.call('HDF5_DISABLE_VERSION_CHECK="1"', shell=True)
    
    repeat = 10
    width = 210
    program_to_execute = './Profiling/cppProcessing2.0 '#Use meaningful data to run # + str(width) + ' ' + str(repeat) 
    subprocess.call('(/usr/bin/time -v ' + program_to_execute + ') &> profile_report.txt', shell=True)
    subprocess.call("echo $'\n' >> profile_report.txt", shell=True)
    subprocess.call('operf '+program_to_execute, shell=True)
    subprocess.call('opreport --symbols >> profile_report.txt', shell=True)
    
    
    f = open('./profile_report.txt', 'r')
    s = f.readline()
    total_time = 0
    percentage_time_ADC_decode = 0
    percentage_time_CDS_correction = 0
    percentage_time_ADU_to_electron_correction = 0
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
            
        elif 'percival_ADC_decode' in s:
            delimited = s.split(' ')
            parsed = [item for item in delimited if item != '']
            percentage_time_ADC_decode = float(parsed[1])
        elif 'percival_CDS_correction'in s:
            delimited = s.split(' ')
            parsed = [item for item in delimited if item != '']
            percentage_time_CDS_correction = float(parsed[1])
        elif 'percival_ADU_to_electron_correction' in s:
            delimited = s.split(' ')
            parsed = [item for item in delimited if item != '']
            percentage_time_ADU_to_electron_correction = float(parsed[1])
        s = f.readline()
        
    function_time_ADC_decode = total_time * percentage_time_ADC_decode /100 /repeat /2
    function_time_CDS_subtraction = total_time * percentage_time_CDS_correction /100 /repeat
    function_time_ADU_to_electron = total_time * percentage_time_ADU_to_electron_correction /100 /repeat
    total_processing_time = function_time_ADC_decode + function_time_ADU_to_electron + function_time_CDS_subtraction
    
    image_size = width * 160 * 2
    if print_result == True:
        print '=' * 100
        print 'The processing functions took in total ' + str(total_processing_time) + 'ms to run.'
        print 'Image size ' + str(width * 160) + '('+str(width) +' * ' + '160)' + ' pixels.' 
        print str(image_size) + ' Bytes.'
        print 'Statistics collected for '+str(repeat)+' iterations'
        print 'Total bandwidth(counting both sample and reset): ' + str(1000 * image_size/total_processing_time /1024/1024) + "MB/s"
        print 'ADC_deocde bandwidth: ' + str(1000 * image_size/function_time_ADC_decode /1024/1024) + "MB/s"
        print 'CDS_subtraction bandwidth: ' + str(1000 * image_size/function_time_CDS_subtraction /1024/1024) + "MB/s"
        print 'ADU_to_electron bandwidth: ' + str(1000 * image_size/function_time_ADU_to_electron /1024/1024) + "MB/s"
        
        print '=' * 100

run_the_function(True)

