#!/bin/env dls-python

import sys
import os
import re
from pkg_resources import require
require('numpy')
import numpy as np
require('h5py')
import h5py

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "need two arguments, infile and outfile"
        sys.exit(1)
    else:
        datafile = sys.argv[1]
        outfile = sys.argv[2]

    if not os.path.exists(datafile):
        print "datafile not found"
        sys.exit(1)

    if not outfile.endswith('.h5') and not outfile.endswith('.hdf5'):
        outfile += '.h5'

    if os.path.exists(outfile):
        print "output file already exists!"
        sys.exit(1)

    infile = file(datafile,'r')

    data_container = {}

    for line in infile:

        # Beginning of new dataset
        if 'Dataset' in line:
            path, _, shape = line.rstrip().split(None, 2 )
            shape = [int(s) for s in shape.strip('{').strip('}').split(',')]
            print path, shape

        # Start of dataset, initialise containers
        elif 'Data:' in line:
            datashape = list(shape)
            # datashape.append(3)
            data_container[path] = np.empty(datashape,dtype=np.int16)

        # Actual data, with indices of first set
        elif line.strip().startswith('('):
            re_indices = re.search('\((\d+,\d+)\)',line)
            iy, ix = [int(x) for x in re_indices.group(0).strip('()').split(',')]
            matches = re.findall('{.+?}',line)
            for i in range(len(matches)):
                fine, coarse, gain = [int(x,16) for x in matches[i].strip('{} ').split(',')]
                # print fine, coarse, gain
                ADC_code = gain + (fine<<2) + (coarse<<10)
                data_container[path][iy, i+ix] = ADC_code
                # print ADC_code

    with h5py.File(outfile,'w') as outfile:
        for key in data_container.keys():
            outfile.create_dataset(key,data=data_container[key])
