
import re
import socket
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
        CPI.append(float(cycle/inst_retired))
    return CPI

def get_bandwidth(list_of_functions, attributes, total_time, image_size, repeat):
    bandwidth = []
    for function in list_of_functions:
        time = float(attributes[function]) /100 * total_time /1000/repeat
        bandwidth.append(2 * image_size/time)
    return bandwidth

def get_time(list_of_functions, attributes, total_time, repeat):
    function_time = []
    for function in list_of_functions:
        time = float(attributes[function]) /100 * total_time/repeat
        if 'ADC' or 'gain' in function:
            time = time / 2
        function_time.append(time)
    return function_time

def get_bytes(bytes):
    suffix = 'Bytes'
    bytes = float(bytes)
    if bytes >= 1024:
        bytes = bytes/1024
        if bytes >= 1024:
            bytes = bytes/1024
            if bytes >= 1024:
                bytes = bytes/1024
                suffix = 'GB'
            else:
                suffix = 'MB'
        else:
            suffix = 'kB'
    return '{:.4}'.format(bytes) + ' ' + suffix

def get_function_list(file):
    list_of_functions = ['percival_ADC_decode(',
                         'percival_ADC_decode_p<tbb::blocked_range<unsigned int> >::operator()',
                         'percival_CDS_correction',
                         'percival_ADU_to_electron_correction', 
                         'percival_unit_ADC_decode(',
                         'percival_unit_ADC_decode_p<tbb::blocked_range<unsigned int> >::operator()',
                         'percival_unit_ADC_decode_p<percival_range_iterator_mock_p>::operator()',
                         'percival_unit_ADC_calibration(',
                         'percival_unit_ADC_calibration_p<tbb::blocked_range<unsigned int> >::operator()',
                         'percival_unit_ADC_calibration_p<percival_range_iterator_mock_p>::operator()',
                         'percival_unit_gain_multiplication(',
                         'percival_unit_gain_multiplication_p<tbb::blocked_range<unsigned int> >::operator()']
    s = file.readline()
    found_functions = []
    while s != '':
        for function_name in list_of_functions:
            if function_name in s:
                found_functions.append(function_name)
        s = file.readline()
    return found_functions

def parse_time(time_string):
    parsed = time_string.split(' ')
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
    return total_time   #return total time in miliseconds

def cmd_call(cmdline):
    subprocess.call(cmdline, shell=True)
    