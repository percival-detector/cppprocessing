
import re
import socket
import subprocess
from pkg_resources import require
require('numpy')
import numpy as np


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
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'LLC_MISSES':
            index_llc_misses = list_of_events_recorded.index(event)
        elif  event.event_name == 'INST_RETIRED':
            index_inst_retired = list_of_events_recorded.index(event)
    
    if 'LLC_MISSES' not in event_names or 'INST_RETIRED' not in event_names:
        return []
    
    for function in list_of_functions:
        llc_misses = attributes[function][index_llc_misses] * list_of_events_recorded[index_llc_misses].sample_rate  
        inst_retired = attributes[function][index_inst_retired] * list_of_events_recorded[index_inst_retired].sample_rate
        llc_misses_per_instruction.append(llc_misses/inst_retired)
    return llc_misses_per_instruction

def get_CPI(list_of_functions, attributes, list_of_events_recorded):
    CPI = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'CPU_CLK_UNHALTED':
            index_cycle = list_of_events_recorded.index(event)
        elif  event.event_name == 'INST_RETIRED':
            index_inst_retired = list_of_events_recorded.index(event)
    
    if 'CPU_CLK_UNHALTED' not in event_names or 'INST_RETIRED' not in event_names:
        return []
    
    for function in list_of_functions:
        cycle = attributes[function][index_cycle] * list_of_events_recorded[index_cycle].sample_rate  
        inst_retired = attributes[function][index_inst_retired] * list_of_events_recorded[index_inst_retired].sample_rate
        CPI.append(float(cycle/inst_retired))
    return CPI

def get_LLC_miss_rate(list_of_functions, attributes, list_of_events_recorded):
    LLC_miss_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'LLC_MISSES':
            index_llc_misses = list_of_events_recorded.index(event)
        elif  event.event_name == 'LLC_REFS':
            index_llc_refs = list_of_events_recorded.index(event)
    
    if 'LLC_MISSES' or 'LLC_REFS' not in event_names:
        return []
    
    for function in list_of_functions:
        llc_misses = attributes[function][index_llc_misses] * list_of_events_recorded[index_llc_misses].sample_rate  
        llc_refs = attributes[function][index_llc_refs] * list_of_events_recorded[index_llc_refs].sample_rate
        LLC_miss_rate.append(float(llc_misses/llc_refs))
    return LLC_miss_rate

def get_DTLB_miss_rate(list_of_functions, attributes, list_of_events_recorded):
    dtlb_load_misses_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'dtlb_load_misses':
            index_dtlb_load_misses = list_of_events_recorded.index(event)
        elif  event.event_name == 'INST_RETIRED':
            index_inst_retired = list_of_events_recorded.index(event)
    
    if 'dtlb_load_misses' not in event_names or 'INST_RETIRED' not in event_names:
        return []
    
    for function in list_of_functions:
        dtlb_load_misses = attributes[function][index_dtlb_load_misses] * list_of_events_recorded[index_dtlb_load_misses].sample_rate  
        inst_retired = attributes[function][index_inst_retired] * list_of_events_recorded[index_inst_retired].sample_rate
        dtlb_load_misses_rate.append(float(dtlb_load_misses/inst_retired))
    return dtlb_load_misses_rate

def get_L1D_miss_rate(list_of_functions, attributes, list_of_events_recorded):
    L1D_miss_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'CPU_CLK_UNHALTED':
            index_cycle = list_of_events_recorded.index(event)
        elif  event.event_name == 'l1d_pend_miss':
            index_l1d_pend_miss = list_of_events_recorded.index(event)
    
    if 'CPU_CLK_UNHALTED' not in event_names or 'l1d_pend_miss' not in event_names:
        return []
    
    for function in list_of_functions:
        cycle = attributes[function][index_cycle] * list_of_events_recorded[index_cycle].sample_rate  
        l1d_pend_miss = attributes[function][index_l1d_pend_miss] * list_of_events_recorded[index_l1d_pend_miss].sample_rate
        L1D_miss_rate.append(float(l1d_pend_miss/cycle))
    return L1D_miss_rate

def get_L1D_repl_rate(list_of_functions, attributes, list_of_events_recorded):
    L1D_repl_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'CPU_CLK_UNHALTED':
            index_cycle = list_of_events_recorded.index(event)
        elif  event.event_name == 'l1d':
            index_l1d_repl = list_of_events_recorded.index(event)
    
    if 'CPU_CLK_UNHALTED' not in event_names or 'l1d' not in event_names:
        return []
    
    for function in list_of_functions:
        cycle = attributes[function][index_cycle] * list_of_events_recorded[index_cycle].sample_rate  
        l1d_repl = attributes[function][index_l1d_repl] * list_of_events_recorded[index_l1d_repl].sample_rate
        L1D_repl_rate.append(float(l1d_repl/cycle))
    return L1D_repl_rate

def get_L2_miss_rate(list_of_functions, attributes, list_of_events_recorded):
    L2_miss_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'l2_lines_in':
            index_L2_lines_in = list_of_events_recorded.index(event)
        elif  event.event_name == 'INST_RETIRED':
            index_inst_retired = list_of_events_recorded.index(event)
    
    if 'l2_lines_in' not in event_names or 'INST_RETIRED' not in event_names:
        return []
    
    for function in list_of_functions:
        L2_lines_in = attributes[function][index_L2_lines_in] * list_of_events_recorded[index_L2_lines_in].sample_rate  
        inst_retired = attributes[function][index_inst_retired] * list_of_events_recorded[index_inst_retired].sample_rate
        L2_miss_rate.append(float(L2_lines_in/inst_retired))
    return L2_miss_rate

def get_br_mispre_rate(list_of_functions, attributes, list_of_events_recorded):
    br_mispre_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'br_inst_retired':
            index_br_inst_retired = list_of_events_recorded.index(event)
        elif  event.event_name == 'br_misp_retired':
            index_br_misp_retired = list_of_events_recorded.index(event)
    
    if 'br_inst_retired' not in event_names or 'br_misp_retired' not in event_names:
        return []
    
    for function in list_of_functions:
        br_inst_retired = attributes[function][index_br_inst_retired] * list_of_events_recorded[index_br_inst_retired].sample_rate  
        br_misp_retired = attributes[function][index_br_misp_retired] * list_of_events_recorded[index_br_misp_retired].sample_rate
        br_mispre_rate.append(float(br_misp_retired/br_inst_retired))
    return br_mispre_rate

def get_resource_stall_rate(list_of_functions, attributes, list_of_events_recorded):
    resource_stall_rate = []
    event_names = []
    for event in list_of_events_recorded:
        event_names.append(event.event_name)    
        if event.event_name == 'CPU_CLK_UNHALTED':
            index_cycle = list_of_events_recorded.index(event)
        elif  event.event_name == 'resource_stalls':
            index_resource_stall = list_of_events_recorded.index(event)
    
    if 'CPU_CLK_UNHALTED' not in event_names or 'resource_stalls' not in event_names:
        return []
    
    for function in list_of_functions:
        cycle = attributes[function][index_cycle] * list_of_events_recorded[index_cycle].sample_rate  
        resource_stall = attributes[function][index_resource_stall] * list_of_events_recorded[index_resource_stall].sample_rate
        resource_stall_rate.append(float(resource_stall/cycle))
    return resource_stall_rate

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
                         'percival_ADC_decode_p<percival_range_iterator_mock_p>::operator()',
                         'percival_CDS_correction',
                         'percival_ADU_to_electron_correction', 
                         'percival_unit_ADC_decode(',
                         'percival_unit_ADC_decode_p<tbb::blocked_range<unsigned int> >::operator()',
                         'percival_unit_ADC_decode_p<percival_range_iterator_mock_p>::operator()',
                         'percival_unit_ADC_calibration(',
                         'percival_unit_ADC_calibration_p<tbb::blocked_range<unsigned int> >::operator()',
                         'percival_unit_ADC_calibration_p<percival_range_iterator_mock_p>::operator()',
                         'percival_unit_gain_multiplication(',
                         'percival_unit_gain_multiplication_p<tbb::blocked_range<unsigned int> >::operator()',
                         'percival_unit_gain_multiplication_p<percival_range_iterator_mock_p>::operator()',
                         'libtbb',
                         'percival_algorithm_p<CDS_output, percival_range_iterator_mock_p>',
                         'ADC_decode_combined_filter<CDS_output>::operator()',
                         'percival_algorithm_p<CDS_output, tbb::blocked_range<unsigned int> >::operator()',
                         'percival_algorithm_p<head_to_CDS, percival_range_iterator_mock_p>::operator()',
                         'ADC_decode_combined_filter::operator()(void*)']
    s = file.readline()
    found_functions = []
    while s != '':
        for function_name in list_of_functions:
            if function_name in s:
                if function_name not in found_functions:
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
    
class accumulator:
    count = 0
    data = []
    parameter_value = []
    file_name = ''
    no_of_samples = 0
    
    def __init__(self, file_name, parameter_value, no_of_samples):
        self.data = []
        self.parameter_value = parameter_value
        self.no_of_samples = no_of_samples
        self.file_name = file_name
        
    def add_number(self, number):
        self.count = self.count + 1
        self.data.append(number)
        
    def average(self):
        array = np.array(self.data)
        mean = np.mean(array)
        std = np.std(array)
        f = open(self.file_name, 'a')
        f.write(str(self.parameter_value) + ':')
        f.write(get_bytes(mean)+ ' ')
        f.write(get_bytes(std)+ ' ')
        for data in self.data:
            f.write(get_bytes(data) + ' ')
        f.write('\n')
        f.close()
        print self.count
    
