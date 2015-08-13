
import re
import subprocess
import socket
import calibration_data_generator as cdg

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

def get_llc_misses(list_of_functions, attributes, list_of_events_recorded):
    llc_misses_per_instruction = []
    for event in list_of_events_recorded:
        if event.event_name == 'LLC_MISSES':
            index_llc_misses = list_of_events_recorded.index(event)
        elif  event.event_name == 'INST_RETIRED':
            index_inst_retired = list_of_events_recorded.index(event)
    for function in list_of_functions:
        llc_misses = attributes[function][index_llc_misses] * list_of_events_recorded[index_llc_misses].sample_rate  
        inst_retired = attributes[function][index_inst_retired] * list_of_events_recorded[index_inst_retired].sample_rate
        llc_misses_per_instruction.append(llc_misses/inst_retired)
    return llc_misses_per_instruction

def get_CPI(list_of_functions, attributes, list_of_events_recorded):
    CPI = []
    for event in list_of_events_recorded:
        if event.event_name == 'CPU_CLK_UNHALTED':
            index_cycle = list_of_events_recorded.index(event)
        elif  event.event_name == 'INST_RETIRED':
            index_inst_retired = list_of_events_recorded.index(event)
    for function in list_of_functions:
        cycle = attributes[function][index_cycle] * list_of_events_recorded[index_cycle].sample_rate  
        inst_retired = attributes[function][index_inst_retired] * list_of_events_recorded[index_inst_retired].sample_rate
        CPI.append(cycle/inst_retired)
    return CPI

def get_bytes(bytes):
    suffix = 'Bytes'
    if bytes > 1024:
        bytes = bytes/1024
        if bytes > 1024:
            bytes = bytes/1024
            if bytes>1024:
                bytes = bytes/1024
                suffix = 'GB'
            else:
                suffix = 'MB'
        else:
            suffix = 'kB'
    return str(int(bytes)) + ' ' + suffix
          

def run_the_function(print_result, height, width, repeat, text_file_name):
    path_name= "./data/KnifeQuadBPos1_2_21_int16.h5"
    top_level_data_set_name= "KnifeQuadBPos1/"
    host_name = socket.gethostname()
    #Program to execute
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '

    cmdl_arg = '1 '  + str(width) + ' '+ str(height) + ' ' + str(repeat) + ' ' + text_file_name    
    program_to_execute = debug_version + cmdl_arg

    #events to monitor
    #instructions
    event1 = oprofile_events('CPU_CLK_UNHALTED','0x00',1000000)
    event2 = oprofile_events('INST_RETIRED','0x00',60000)
    #cache misses
    event3 = oprofile_events('LLC_MISSES','0x41',6000)
#     event4 = oprofile_events('mem_load_uops_llc_hit_retired','0x02',100000)
#     event5 = oprofile_events('mem_load_uops_llc_hit_retired','0x04',100000)
#     event6 = oprofile_events('mem_load_uops_retired','0x04',2000000)
    event4 = oprofile_events('UNHALTED_REFERENCE_CYCLES','0x01',100000)
    
    list_of_events = [event1, event2, event3, event4]#, event5, event6, event7]
    
    dict_of_attributes = {}
    list_of_functions = ['percival_ADC_decode', 'percival_CDS_correction','percival_ADU_to_electron_correction']
    list_of_events_recorded = []

    operf_events = get_operf_option(list_of_events)
    
    
    result_directory = './oprof_reports/'+ host_name + '/'
    report_destination = result_directory + 'profile_report.txt'
    sample_data_destination = result_directory + 'oprof_data/'
    print 'operf ' + '-d ' + sample_data_destination + ' ' + operf_events + ' '+ program_to_execute
    
    subprocess.call('mkdir -p ' + sample_data_destination, shell=True)
    
    #subprocess.call('export LD_LIBRARY_PATH=/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-14/prefix/lib', shell=True)
    subprocess.call('(/usr/bin/time -v ' + program_to_execute + ') &> ' + report_destination, shell=True)
    subprocess.call("echo $'\n' >> " + report_destination, shell=True)
    subprocess.call('operf ' + '-d ' + sample_data_destination + ' ' + operf_events + ' '+ program_to_execute, shell=True)
    subprocess.call('opreport --symbols --session-dir=' + sample_data_destination +  ' >> ' + report_destination, shell=True)
    
    #subprocess.call('opannotate -s --output-dir=annotated ' + program_to_execute, shell=True)
    
    f = open(report_destination, 'r')
    s = f.readline()
    total_time = 0
    dict_of_function_perc_time = {}
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
        for function_name in list_of_functions:
            if function_name in s:
                delimited = s.split(' ')
                parsed = [item for item in delimited if item != '']
                attributes = []
                dict_of_function_perc_time[function_name] = float(parsed[1])
                for index in range(len(list_of_events_recorded)):  # manually add the percentage clock cycles
                    attributes.append(float(parsed[index * 2]))
                dict_of_attributes[function_name] = attributes
        s = f.readline()
        
    function_time_ADC_decode = float(total_time * dict_of_function_perc_time['percival_ADC_decode'] /100 /repeat)
    function_time_CDS_subtraction = float(total_time * dict_of_function_perc_time['percival_CDS_correction'] /100 /repeat)
    function_time_ADU_to_electron = float(total_time * dict_of_function_perc_time['percival_ADU_to_electron_correction'] /100 /repeat)
    total_processing_time = float(function_time_ADC_decode + function_time_ADU_to_electron + function_time_CDS_subtraction)
    
    llc_misses_per_instruction = get_llc_misses(list_of_functions, dict_of_attributes, list_of_events_recorded)
    CPI = get_CPI(list_of_functions, dict_of_attributes, list_of_events_recorded)
    image_size = width * height * 2    #memory size
    
    if print_result == True:
        print '=' * 100
        print 'operf ' + '-d ' + sample_data_destination + ' ' + operf_events + ' '+ program_to_execute
        print 'The program took {0:.4} ms in total. {1:.4} ms per sample/reset pair.'.format(total_time, total_time/repeat)
        print 'Of which the processing functions took in total {0:.4} ms to run.'.format(total_processing_time)
        print 'Image size {0:d} ({1:d} * {2:d}) pixels.'.format(width * height, height, width), 
        print get_bytes(image_size)
        print 'Statistics collected for '+str(repeat)+' iterations'
        
        print 'Bandwidth:'
        print '\t Total: {0:.4} MB/s'.format(1000 * image_size * 2/total_processing_time /1024/1024)    #2 because of sample and reset
        print '\t ADC_deocde bandwidth: {0:.4} MB/s'.format(1000 * image_size * 2/function_time_ADC_decode /1024/1024)
        print '\t CDS_subtraction (two float input):  {0:.4} MB/s'.format(1000 * 2 * 2 * image_size/function_time_CDS_subtraction /1024/1024) #2 for float, 2 for sample and reset
        print '\t ADU_to_electron (one float input):  {0:.4} MB/s'.format(1000 * 2 * image_size/function_time_ADU_to_electron /1024/1024)
        
        print 'LLC misses per instruction:'
        for function in list_of_functions:
            index = list_of_functions.index(function)
            print '\t' +  function +':' + '{0:.2%}'.format(llc_misses_per_instruction[index]) 
            
        print 'Cycle per instruction (CPI):'
        for function in list_of_functions:
            index = list_of_functions.index(function)
            print '\t' +  function +':' + '{0}'.format(int(CPI[index])) 
        print '=' * 100



repeat = 1    #350 is about the maximum
# width_arr = [2000, 5000, 10000, 20000, 50000, 100000, 500000]

height = 3717
width = 3528

host_name = socket.gethostname()
calib_directory = './calibration_data/'
path_name = calib_directory + host_name + '/'
text_file_name =  path_name + 'test_param_file.txt'

subprocess.call('mkdir -p ' + path_name, shell=True)



cdg.generate_calib_files(height, width, text_file_name, path_name)

# for width in width_arr:
run_the_function(True, height, width, repeat, text_file_name)


# width = 50000
# repeat_arr = [1,2,5,10,20,50,100]
# for repeat in repeat_arr:
#     run_the_function(True, width, repeat)







#operf -e CPU_CLK_UNHALTED=100000:0:1:1,mem_load_uops_llc_hit_retired=100000:2:1:1,mem_load_uops_llc_hit_retired=100000:4:1:1,mem_load_uops_retired=100000:4:1:1, ./Debug/cppProcessing2.0 0 ./data/KnifeQuadBPos1_2_21_int16.h5 KnifeQuadBPos1/ 500


