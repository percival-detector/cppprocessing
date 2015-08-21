
import re
import subprocess
import socket
import calibration_data_generator as cdg    
import output_processing as op
import datetime

def run_the_function(print_result, height, width, repeat, text_file_name):
    path_name= "./data/KnifeQuadBPos1_2_21_int16.h5"
    top_level_data_set_name= "KnifeQuadBPos1/"
    host_name = socket.gethostname()
    #Program to execute
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '
    parallel_debug = './parallel_debug/cppProcessing2.0 '
    parallel_profile = './parallel_profile/cppProcessing2.0 '

    cmdl_arg = '1 '  + str(width) + ' '+ str(height) + ' ' + str(repeat) + ' ' + text_file_name    
    program_to_execute = parallel_debug + cmdl_arg

    #events to monitor
    #instructions
    event1 = op.oprofile_events('CPU_CLK_UNHALTED','0x00',100000000)
    event2 = op.oprofile_events('INST_RETIRED','0x00',6000000)
    #cache misses
    event3 = op.oprofile_events('LLC_MISSES','0x41',60000)
#     event4 = oprofile_events('mem_load_uops_llc_hit_retired','0x02',100000)
#     event5 = oprofile_events('mem_load_uops_llc_hit_retired','0x04',100000)
#     event6 = oprofile_events('mem_load_uops_retired','0x04',2000000)
    event4 = op.oprofile_events('UNHALTED_REFERENCE_CYCLES','0x01',1000000)
    
    list_of_events = [event1, event2, event3, event4]#, event5, event6, event7]
    #variable initialisation
    dict_of_attributes = {}
    total_time = 0.0
    dict_of_function_perc_time = {}
    list_of_events_recorded = []
    image_size = width * height * 2    #memory size
    total_processing_time = 0.0


    operf_events = op.get_operf_option(list_of_events)
    
    
    result_directory = './oprof_reports/'+ host_name + '/'
    report_destination = result_directory + 'profile_report.txt'
    sample_data_destination = result_directory + 'oprof_data/'
    
    #commandline options
    cmd_git = 'git rev-parse HEAD > ' + report_destination
    cmd_time = '(/usr/bin/time -v ' + program_to_execute + ') &>> ' + report_destination
    cmd_operf = 'operf ' + '-d ' + sample_data_destination + ' ' + operf_events + ' '+ program_to_execute
    cmd_opreport = 'opreport --symbols --session-dir=' + sample_data_destination +  ' >> ' + report_destination
    cmd_mkdir = 'mkdir -p ' + sample_data_destination
    
    op.cmd_call(cmd_git)
    op.cmd_call(cmd_time)
    op.cmd_call(cmd_operf)
    op.cmd_call(cmd_opreport)
    op.cmd_call(cmd_mkdir)
    
#     subprocess.call(cmd_git, shell=True)
#     subprocess.call(cmd_mkdir, shell=True)
#     #subprocess.call('export LD_LIBRARY_PATH=/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-14/prefix/lib', shell=True)
#     subprocess.call(cmd_time, shell=True)
#     subprocess.call(cmd_operf, shell=True)
#     subprocess.call(cmd_opreport, shell=True)
    
#     subprocess.call('opannotate -s --output-dir=' + result_directory + 'annotated/ ' + program_to_execute, shell=True)
    
    f = open(report_destination, 'r')
    list_of_functions = op.get_function_list(f)
    f.close()
    
    f = open(report_destination, 'r')
    s = f.readline()
    commit_key = s;
    while s != '':
	if 'Counted' in s:
	    for event in list_of_events:
		if event.event_name in s:
  		    list_of_events_recorded.append(event)
        if 'Elapsed (wall clock) time ' in s:
            total_time = op.parse_time(s)
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
    
    print total_time
    print list_of_functions
    print attributes
    
    llc_misses_per_instruction = op.get_llc_misses(list_of_functions, dict_of_attributes, list_of_events_recorded)
    CPI = op.get_CPI(list_of_functions, dict_of_attributes, list_of_events_recorded)
    bandwidth = op.get_bandwidth(list_of_functions, dict_of_function_perc_time, total_time, image_size, repeat)
    function_time = op.get_time(list_of_functions, dict_of_function_perc_time, total_time, repeat)

    for name, perc_time in dict_of_function_perc_time.iteritems():
        total_processing_time = total_processing_time + perc_time * total_time /1000 /100   #in seconds
        print total_processing_time
        
    print function_time
    print llc_misses_per_instruction
    print CPI
    #printing reports
    if print_result == True:
        print
        print datetime.datetime.now().date(),
        print datetime.datetime.now().time(),
        print 'Report:'
        print cmd_time
        print cmd_operf
        print cmd_opreport
        print 'oprofile sample directory: ' + sample_data_destination

        print '=' * 100
        print commit_key,
        print 'operf ' + '-d ' + sample_data_destination + ' ' + operf_events + ' '+ program_to_execute
        print 'The program took {0:.4} ms in total. {1:.4} ms per sample/reset pair.'.format(total_time, total_time/repeat)
        print 'Of which the processing functions took in total {0:.4} ms to run.'.format(total_processing_time)
        print 'Image size {0:d} ({1:d} * {2:d}) pixels.'.format(width * height, height, width), 
        print op.get_bytes(image_size)
        print 'Statistics collected for '+str(repeat)+' iterations'
        
        print 'Bandwidth:'
        print '\t' + 'Total: {0:.4} MB/s'.format(op.get_bytes(image_size * 2 * repeat/total_processing_time) )   #2 because of sample and reset
        for function in list_of_functions:
            index = list_of_functions.index(function)
            print '\t' +  function +':' + '{0}'.format(op.get_bytes((bandwidth[index])))   + '/s'             
        print 'LLC misses per instruction:'
        for function in list_of_functions:
            index = list_of_functions.index(function)
            print '\t' +  function +':' + '{0:.2%}'.format(llc_misses_per_instruction[index]) 
            
        print 'Cycle per instruction (CPI):'
        for function in list_of_functions:
            index = list_of_functions.index(function)
            print '\t' +  function +':' + '{0:.2}'.format(CPI[index]) 
            
        print 'Processing time (ms) for each call:'
        for function in list_of_functions:
            index = list_of_functions.index(function)
            print '\t' +  function +':' + '{0}'.format(int(function_time[index]))                
        print '=' * 100



repeat = 50   #350 is about the maximum
# width_arr = [2000, 5000, 10000, 20000, 50000, 100000, 500000]

height = 3717
width = 3528

host_name = socket.gethostname()
calib_directory = './calibration_data/'
path_name = calib_directory + host_name + '/'
text_file_name =  path_name + 'test_param_file.txt'
 


# subprocess.call('mkdir -p ' + path_name, shell=True)
# cdg.generate_calib_files(height, width, text_file_name, path_name)

# for width in width_arr:
run_the_function(True, height, width, repeat, text_file_name)


# width = 50000
# repeat_arr = [1,2,5,10,20,50,100]
# for repeat in repeat_arr:
#     run_the_function(True, width, repeat)







#operf -e CPU_CLK_UNHALTED=100000:0:1:1,mem_load_uops_llc_hit_retired=100000:2:1:1,mem_load_uops_llc_hit_retired=100000:4:1:1,mem_load_uops_retired=100000:4:1:1, ./Debug/cppProcessing2.0 0 ./data/KnifeQuadBPos1_2_21_int16.h5 KnifeQuadBPos1/ 500


