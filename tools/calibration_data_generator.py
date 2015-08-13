from pkg_resources import require
require('h5py')
import h5py
import numpy as np
import random

def generate_array(height, width, lower_bound, upper_bound):
    Arr = np.zeros((height, width))
    for indexw in range(width):
        for indexh in range(height):
            Arr[indexh][indexw] = random.uniform(lower_bound, upper_bound)
    return Arr

class calibration_parameters:
    parameter_name = ''
    height = 1
    width = 1
    lower_bound = 0
    upper_bound = 10
    file_name = ''
    dataset_name = 'data/data'
    datatype = ''
    
    def __init__(self, parameter_name, height, width, lower_bound, upper_bound, datatype):
        self.parameter_name = parameter_name
        self.height = height
        self.width = width
        self.lower_bound = lower_bound
        self.upper_bound = upper_bound
        self.file_name = self.parameter_name + '.h5'
        self.datatype = datatype
        
    def write_to_hdf5(self):
        file = h5py.File(self.file_name, 'w')
        dset = file.create_dataset(self.dataset_name, data=generate_array(self.height, self.width, self.lower_bound, self.upper_bound), dtype=self.datatype)
        
    def write_to_text_file(self, master_file_name):
        file = open(master_file_name, 'a')
        str = 'default_location_' + self.parameter_name + ':\"' + self.file_name + '\"\n'
        file.write(str)
        file.close()


def generate_calib_files(height, width, text_file_name):
    
    file = open(text_file_name, 'w')
    file.write('default_calib_params_dataset_name:\"/data/data\"\n')
    file.write('is_transposed_calib_params:yes\n')
    file.close()
    
    #'calib_params_dataset_name':    "/data/data"
    param1 = calibration_parameters ('Gc', 7, height, 0, 20, 'float32')
    param2 = calibration_parameters ('Oc', 7, height, 0, 20, 'float32')
    param3 = calibration_parameters ('Gf', 7, height, 0, 20, 'float32')
    param4 = calibration_parameters ('Of', 7, height, 0, 20, 'float32')
    param5 = calibration_parameters ('ADU_to_electrons_conversion', width, height, 0, 20, 'float32')
    
    param6 = calibration_parameters ('Gain_lookup_table1', width, height, 0, 20, 'float32')
    param7 = calibration_parameters ('Gain_lookup_table2', width, height, 0, 20, 'float32')
    param8 = calibration_parameters ('Gain_lookup_table3', width, height, 0, 20, 'float32')
    param9 = calibration_parameters ('Gain_lookup_table4', width, height, 0, 20, 'float32')
    
    list_of_parameters = [param1, param2, param3, param4, param5, param6, param7, param8, param9]
    
    for params in list_of_parameters:
        params.write_to_hdf5()
        params.write_to_text_file(text_file_name)


#generate Gc Gf Oc Of gain table ADU to electron convertion.


