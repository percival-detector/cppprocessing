
import re
import subprocess

class oprofile_events:
    event_name = ''
    unit_mask = ''
    sample_rate = 10000
    def __init__(self, event_name, unit_mask, sample_rate):
	self.event_name = event_name
	self.unit_mask = unit_mask
	self.sample_rate = sample_rate
    def getString(self):
	return self.event_name + ':' +str(self.sample_rate)+':'+self.unit_mask+':1:1'

def get_operf_option(list):
    str = '-e '
    for event in list:
	str = str + event.getString()
    	str = str + ','
    return str[:-1]

def run_the_function(print_result):
    #subprocess.call('LD_LIBRARY_PATH="/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-15/prefix/lib"', shell=True)
    #subprocess.call('HDF5_DISABLE_VERSION_CHECK="1"', shell=True)
    path_name= "./data/KnifeQuadBPos1_2_21_int16.h5"
    top_level_data_set_name= "KnifeQuadBPos1/"
    
    repeat = 500
    width = 1024
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '

    #program_to_execute = './Profiling/cppProcessing2.0 '#Use meaningful data to run # + str(width) + ' ' + str(repeat) 
#    program_to_execute = './Debug/cppProcessing2.0 ' + "0 "+ path_name + " " + top_level_data_set_name + " " + str(repeat)
    cmdl_arg = '1 ' + str(width) + ' ' + str(repeat)    
    program_to_execute = debug_version + cmdl_arg

    #events to monitor
    #clock cycle
    cpu_clock_sample_counts = 100000
    unit_mask = 0x00
    CPU_CLK_UNHALTED = 'CPU_CLK_UNHALTED:' + str(cpu_clock_sample_counts) + ':' + str(unit_mask) + ':1:1,'
    #cache miss
    event1 = oprofile_events('CPU_CLK_UNHALTED','0x00',100000)
    event2 = oprofile_events('INST_RETIRED','0x00',6000)
    event3 = oprofile_events('mem_load_uops_llc_hit_retired','0x02',100000)
    event4 = oprofile_events('mem_load_uops_llc_hit_retired','0x04',100000)
    event5 = oprofile_events('mem_load_uops_retired','0x04',2000000)
 
    list_of_events = [event1, event2, event3, event4, event5]
    list_of_attributes = []
#    mem_load_uops_llc_hit_retired_0x02_sample_counts = 100000                #100000 min
#    unit_mask = 0x02
#    mem_load_uops_llc_hit_retired_0x02 = 'mem_load_uops_llc_hit_retired:' + str(mem_load_uops_llc_hit_retired_0x02_sample_counts) + ':' + str(unit_mask) + ':1:1,'
    

#    mem_load_uops_llc_hit_retired_0x04_sample_counts = 100000                #100000 min
#    unit_mask = 0x04
#    mem_load_uops_llc_hit_retired_0x04 = 'mem_load_uops_llc_hit_retired:' + str(mem_load_uops_llc_hit_retired_0x04_sample_counts) + ':' + str(unit_mask) + ':1:1,'
    
#    mem_load_uops_retired_0x04_sample_counts = 2000000                #2000000 min
#    unit_mask = 0x04
#    mem_load_uops_retired_0x04 = 'mem_load_uops_retired:' + str(mem_load_uops_retired_0x04_sample_counts) + ':' + str(unit_mask) + ':1:1,'
    
#    operf_events = '-e ' + mem_load_uops_llc_hit_retired_0x02 + mem_load_uops_llc_hit_retired_0x04 + mem_load_uops_retired_0x04
 
    operf_events = get_operf_option(list_of_events)
    print 'operf ' + operf_events + ' '+ program_to_execute
    
    subprocess.call('(/usr/bin/time -v ' + program_to_execute + ') &> profile_report.txt', shell=True)
    subprocess.call("echo $'\n' >> profile_report.txt", shell=True)
    subprocess.call('operf ' + operf_events + ' '+ program_to_execute, shell=True)
    subprocess.call('opreport --symbols >> profile_report.txt', shell=True)
    
    #subprocess.call('opannotate -s --output-dir=annotated ' + program_to_execute, shell=True)
    
    f = open('./profile_report.txt', 'r')
    s = f.readline()
    total_time = 0
    percentage_time_ADC_decode = 0
    percentage_time_CDS_correction = 0
    percentage_time_ADU_to_electron_correction = 0
    while s != '':
	if 'Counted' in s:
	    for event in list_of_events:
		if event.event_name in s:
  		    list_of_events_recorded.append(event)
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
            total_time = float(hrs + msc + sec + min)
            
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
        
    function_time_ADC_decode = float(total_time * percentage_time_ADC_decode /100 /repeat)
    function_time_CDS_subtraction = float(total_time * percentage_time_CDS_correction /100 /repeat)
    function_time_ADU_to_electron = float(total_time * percentage_time_ADU_to_electron_correction /100 /repeat)
    total_processing_time = float(function_time_ADC_decode + function_time_ADU_to_electron + function_time_CDS_subtraction)
    
    image_size = width * 160 * 2
    if print_result == True:
        print '=' * 100
        print 'The program took ' + str(total_time/repeat) + 'ms per sample/reset pair.'
        print 'Of which the processing functions took in total ' + str(total_processing_time) + 'ms to run.'
        print 'Image size ' + str(width * 160) + '('+str(width) +' * ' + '160)' + ' pixels.', 
        print str(image_size) + ' Bytes.'
        print 'Statistics collected for '+str(repeat)+' iterations'
        print 'Total bandwidth(counting both sample and reset): ' + str(1000 * image_size * 2/total_processing_time /1024/1024) + "MB/s"    #2 because of sample and reset
        print 'ADC_deocde bandwidth: ' + str(1000 * image_size * 2/function_time_ADC_decode /1024/1024) + "MB/s"
        print 'CDS_subtraction bandwidth (two float input): ' + str(1000 * 2 * 2 * image_size/function_time_CDS_subtraction /1024/1024) + "MB/s" #2 for float, 2 for sample and reset
        print 'ADU_to_electron bandwidth (one float input): ' + str(1000 * 2 * image_size/function_time_ADU_to_electron /1024/1024) + "MB/s"
        print '=' * 100

run_the_function(True)






#operf -e CPU_CLK_UNHALTED=100000:0:1:1,mem_load_uops_llc_hit_retired=100000:2:1:1,mem_load_uops_llc_hit_retired=100000:4:1:1,mem_load_uops_retired=100000:4:1:1, ./Debug/cppProcessing2.0 0 ./data/KnifeQuadBPos1_2_21_int16.h5 KnifeQuadBPos1/ 500


