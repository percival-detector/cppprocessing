
import re
import subprocess
import socket
import calibration_data_generator as cdg    
import output_processing as op
import datetime

def run_the_function(print_result, height, width, repeat, text_file_name, grain_size):
    path_name= "./data/KnifeQuadBPos1_2_21_int16.h5"
    top_level_data_set_name= "KnifeQuadBPos1/"
    host_name = socket.gethostname()
    #Program to execute
    debug_version = './Debug/cppProcessing2.0 '
    profile_version = './Profiling/cppProcessing2.0 '
    parallel_debug = './parallel_debug/cppProcessing2.0 '
    parallel_profile = './parallel_profile/cppProcessing2.0 '

    cmdl_arg = '1 '  + str(width) + ' '+ str(height) + ' ' + str(repeat) + ' ' + text_file_name + ' ' + str(grain_size) + ' 1'
    program_to_execute = parallel_profile + cmdl_arg

    #events to monitor
    #instructions
    event1 = op.oprofile_events('CPU_CLK_UNHALTED','0x00',100000000)
    event2 = op.oprofile_events('INST_RETIRED','0x00',60000000)
    #cache misses
    event3 = op.oprofile_events('LLC_MISSES','0x41',60000)          #divide by LLC_REFS
#     event4 = op.oprofile_events('l2_lines_in','0x07',1000000)       #100000
    
#     event5 = op.oprofile_events('br_inst_retired', '0x01', 400000)    #total branch instructions retired
#     event6 = op.oprofile_events('br_misp_retired', '0x01', 400000)    #total mispredicted branches. Divide by br_inst_retired
    
    event6 = op.oprofile_events('mem_trans_retired','0x02',2000000)
#     event7 = op.oprofile_events('uops_retired', 'stall_cycles',2000000)        #no of stall cycles. Divide by cpu cycles
#     
#     event8 = op.oprofile_events('dtlb_load_misses', '0x01',2000000) 
# #    event8 = op.oprofile_events('dtlb_load_misses', '0x81',1000)   #Ivy Bridge
#         
#     event9 = op.oprofile_events('LLC_REFS', '0x4f',6000)            
#  
#     event10 = op.oprofile_events('l1d_pend_miss', 'pending',2000000)        #cycles of l1d misses outstanding. Divide by CPU cycles
#     event11 = op.oprofile_events('resource_stalls', '0x01',2000000)        #no of stall cycles/divide by number of instructions
#     event12 = op.oprofile_events('l1d', '0x01',2000000)        #cycles of l1d misses outstanding. Divide by CPU cycles

    list_of_events = [event1, event2, event3, event6]#, event7, event8, event9, event10, event11, event12]
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
    cmd_annotate = 'opannotate -s --output-dir=' + result_directory + 'annotated/ ' + '--session-dir=' + sample_data_destination + ' '+ program_to_execute
    
    op.cmd_call(cmd_git)
    op.cmd_call(cmd_time)
    op.cmd_call(cmd_operf)
    op.cmd_call(cmd_opreport)
    op.cmd_call(cmd_mkdir)
    op.cmd_call(cmd_annotate)
    
    f = open(report_destination, 'r')
    list_of_functions = op.get_function_list(f)
    f.close()
    for function_name in list_of_functions:
        dict_of_function_perc_time[function_name] = 0
        dict_of_attributes[function_name] = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    
    f = open(report_destination, 'r')
    s = f.readline()
    commit_key = s;
    while s != '':
    	if 'Counted' in s:
    	    for event in list_of_events:
    		if (event.event_name + ' events') in s:
      		    list_of_events_recorded.append(event)
        if 'Elapsed (wall clock) time ' in s:
            total_time = op.parse_time(s)
        for function_name in list_of_functions:
            if function_name in s:
                delimited = s.split(' ')
                parsed = [item for item in delimited if item != '']
                attributes = []
                dict_of_function_perc_time[function_name] = float(parsed[1]) + dict_of_function_perc_time[function_name]
                for index in xrange(len(list_of_events_recorded)):  # manually add the percentage clock cycles
                    attributes.append( float(parsed[index * 2]) + dict_of_attributes[function_name][index] )      
                dict_of_attributes[function_name] = attributes
        s = f.readline()

    llc_misses_per_instruction = op.get_llc_misses(list_of_functions, dict_of_attributes, list_of_events_recorded)
    CPI = op.get_CPI(list_of_functions, dict_of_attributes, list_of_events_recorded)
    bandwidth = op.get_bandwidth(list_of_functions, dict_of_function_perc_time, total_time, image_size, repeat)
    function_time = op.get_time(list_of_functions, dict_of_function_perc_time, total_time, repeat)
    l1d_miss_rate = op.get_L1D_miss_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    DTLB_miss_rate = op.get_DTLB_miss_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    LLC_miss_rate = op.get_LLC_miss_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    br_misspre_rate = op.get_br_mispre_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    resource_stall = op.get_resource_stall_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    l2_miss_rate = op.get_L2_miss_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    l1d_repl_rate = op.get_L1D_repl_rate(list_of_functions, dict_of_attributes, list_of_events_recorded)
    memory_bandwidth = op.get_memory_bandwidth(list_of_functions, dict_of_attributes, list_of_events_recorded)
    
    for name, perc_time in dict_of_function_perc_time.iteritems():
        total_processing_time = total_processing_time + perc_time * total_time /1000 /100   #in seconds

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
        print cmd_annotate

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
        
        if len(l1d_miss_rate) != 0:
            print 'L1D misses percentage:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2%}'.format(l1d_miss_rate[index]) 
                
        if len(l1d_repl_rate) != 0:
            print 'L1D replacement rate:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2%}'.format(l1d_repl_rate[index]) 
                
        if len(l2_miss_rate) != 0:
            print 'L2 misses percentage:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2%}'.format(l2_miss_rate[index]) 
        
        if len(DTLB_miss_rate) != 0:
            print 'DTLB miss per instruction:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2}'.format(DTLB_miss_rate[index]) 
        
        if len(LLC_miss_rate) != 0:
            print 'LLC miss rate:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2%}'.format(LLC_miss_rate[index]) 
        
        if len(br_misspre_rate) != 0:     
            print 'Branch misprediction percentage:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2%}'.format(br_misspre_rate[index]) 
        
        if len(resource_stall) != 0:
            print 'Resource stall cycle percentage:'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + '{0:.2%}'.format(resource_stall[index]) 
               
        if len(memory_bandwidth) != 0:
            print 'Memory Bandwidth'
            for function in list_of_functions:
                index = list_of_functions.index(function)
                print '\t' +  function +':' + op.get_bytes(memory_bandwidth[index]) 
                             
        print '=' * 100
    return image_size * 2 * repeat/total_processing_time


repeat = 100   #350 is about the maximum
# width_arr = [2000, 5000, 10000, 20000, 50000, 100000, 500000]

height = 3717
width = 3528

host_name = socket.gethostname()
calib_directory = './calibration_data/'
path_name = calib_directory + host_name + '/'
text_file_name =  path_name + 'test_param_file.txt'
grain_size_file_name = './oprof_reports/' + host_name + '/' + 'grain_size_test.txt'
print grain_size_file_name 
 
 
# subprocess.call('mkdir -p ' + path_name, shell=True)
# cdg.generate_calib_files(height, width, text_file_name, path_name)
          
# cmd_rm_file = "rm -f " + grain_size_file_name
# cmd_create_file = "touch "+ grain_size_file_name
# op.cmd_call(cmd_rm_file)
# op.cmd_call(cmd_create_file)
  
grain_size = 3528 * 2
run_the_function(True, height, width, repeat, text_file_name, grain_size)
#  1,3,7,9,21, 59,63,177,413,531,
#  *      1239,3717
# for grain_size in (3528, 3528/2,3528/3,3528/4,3528/6,3528/7, 3528*3, 3528*7,3528*9, 3528*21, 3528*59, 3528*63, 3528*177,3528*413,3528*531,3528*1239):
#     a = op.accumulator(grain_size_file_name, grain_size, 2)
#     for repeats in xrange(1,2):
#         bandwidth = run_the_function(True, height, width, repeat, text_file_name, grain_size)
#         a.add_number(bandwidth)
#         print grain_size, op.get_bytes(bandwidth)
#     a.average()
#     
    
    
# print 'starting'
# grain_size = 20
# run_the_function(True, height, width, repeat, text_file_name, grain_size)

# for index in xrange(9, 41):
#     run_the_function(True, height, width, repeat, text_file_name, index)
#     print index
 
# for grain_size in xrange(10000,1000000,2000):
#     a = op.accumulator(grain_size_file_name, grain_size, 9)
#     for repeats in xrange(1,9):
#         bandwidth = run_the_function(False, height, width, repeat, text_file_name, grain_size)
#         a.add_number(bandwidth[0])
#         print grain_size, op.get_bytes(bandwidth[0])
#     a.average()
# #     



# width = 50000
# repeat_arr = [1,2,5,10,20,50,100]
# for repeat in repeat_arr:
#     run_the_function(True, width, repeat)







#operf -e CPU_CLK_UNHALTED=100000:0:1:1,mem_load_uops_llc_hit_retired=100000:2:1:1,mem_load_uops_llc_hit_retired=100000:4:1:1,mem_load_uops_retired=100000:4:1:1, ./Debug/cppProcessing2.0 0 ./data/KnifeQuadBPos1_2_21_int16.h5 KnifeQuadBPos1/ 500


