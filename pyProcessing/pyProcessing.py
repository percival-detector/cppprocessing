#!/dls_sw/prod/tools/RHEL6-x86_64/defaults/bin/dls-python2.7
# This is a copy of Hazam Yousef's script of the same name.
# His version is run inside the DAWN framework, this one uses dls-python

import dls_packages
import sys
from Cython.Shadow import typeof
sys.executable = ''
from pkg_resources import require
require('matplotlib')
import matplotlib.pylab as plt
require('h5py')
import numpy as np
import os
import math


#import dls_packages  # required for h5py on dls systems
import h5py


class calibParams:
    def __init__(self, ADCrows, rows, cols):
        self.GC = np.ones([ADCrows, cols], dtype='float32')
        self.OC = np.zeros([ADCrows, cols], dtype='float32')
        self.GF = np.ones([ADCrows, cols], dtype='float32')
        self.OF = np.zeros([ADCrows, cols], dtype='float32')
        self.rows = rows
        self.cols = cols
        self.ADCrows = ADCrows
        self.ADUs_per_el = np.zeros([rows, cols], dtype='float32')

    def Load_ADC_Calib_H5Files(self, ADC_calibration_file):
        CG_file_name = ADC_calibration_file + 'CoarseGainArray.h5'
        CO_file_name = ADC_calibration_file + 'CoarseOffsetArray.h5'
        FG_file_name = ADC_calibration_file + 'FineGainArray.h5'
        FO_file_name = ADC_calibration_file + 'FineOffsetArray.h5'

        CG_file = h5py.File(CG_file_name, 'a')
        CO_file = h5py.File(CO_file_name, 'a')
        FG_file = h5py.File(FG_file_name, 'a')
        FO_file = h5py.File(FO_file_name, 'a')

        self.GC = CG_file['/data/data'][:]
        self.OC = CO_file['/data/data'][:]
        self.GF = FG_file['/data/data'][:]
        self.OF = FO_file['/data/data'][:]

        self.GC = np.tile(self.GC, (self.rows / self.ADCrows, 1))
        self.OC = np.tile(self.OC, (self.rows / self.ADCrows, 1))
        self.GF = np.tile(self.GF, (self.rows / self.ADCrows, 1))
        self.OF = np.tile(self.OF, (self.rows / self.ADCrows, 1))

        self.CMax = math.ceil(self.GC.max())
        self.FMax = (math.ceil(self.GF.max() * 0.065))
        
        print self.CMax, self.FMax

    def Load_ADU2e_Calib_H5Files(self, ADU2E_calibration_file):
        ADU2e_file = h5py.File(ADU2E_calibration_file)
        self.ADUs_per_el = ADU2e_file['/data/data'][:]


class Frame:
    def __init__(self, rows, cols):
        self.CoarseArr = np.zeros([rows, cols], dtype='int16')
        self.FineArr = np.zeros([rows, cols], dtype='int16')
        self.GainArr = np.zeros([rows, cols], dtype='int16')
        self.ADUCorr = np.zeros([rows, cols], dtype='int16')

    def TxTFrameReader(self, fpath, filename, groupname, dataType, imageNumber, ADCCodeName):
        get_file = os.path.join(fpath + filename)
        data_string = groupname + '/' + str(imageNumber) + '/' + dataType
        adc_code_switcher = {
            "coarse" : 0,
            "fine" : 1,
            "gain" : 2,
        }
        adc_index = adc_code_switcher[ADCCodeName]
        # print 'get_file=', get_file, adc_index
        if not os.path.isfile(get_file):
            raise IOError('File does not exist.')
        with h5py.File(get_file) as got_file:
            Array = np.transpose(got_file[data_string][:, :, adc_index])
        return Array

    def ADUcorrection(self, calibParams):
        VinMax = 1.43
        self.ADUCorr = p.FMax * p.CMax * (1.0 - ((1.0 / VinMax) * (((calibParams.OC - self.CoarseArr - 1.0) / (calibParams.GC)) + ((self.FineArr - calibParams.OF) / calibParams.GF))))
        return self.ADUCorr


class data:
    def __init__(self, rows, cols):
        self.sample = Frame(rows, cols)
        self.reset = Frame(rows, cols)
        self.CDSimg = np.zeros([rows, cols], dtype='float')
        self.eImg = np.zeros([rows, cols], dtype='float')
        return

    def CDSimage(self):
        # The CDS is applied only when the gain is 1 (Diode only)
        self.CDSimg = self.sample.ADUCorr - self.reset.ADUCorr
        return

    def ADU2eCor(self, calibParams):
        self.eImg = self.CDSimg / calibParams.ADUs_per_el
        return self.eImg


if __name__ == '__main__':

    d = data(210, 160)
    # print "\n".join(vars(data))

    inpath = './data/'
    ifilename = 'KnifeQuadBPos1_2_21_int8.h5'
    igroupname = 'KnifeQuadBPos1'
    idataType = 'Sample'
    iimageNumber = 10

    print ('Process the Sample data ...\n')
    d.sample = Frame(210, 160)
    d.sample.CoarseArr = d.sample.TxTFrameReader(inpath, ifilename, igroupname, idataType, iimageNumber, 'coarse')
    d.sample.FineArr = d.sample.TxTFrameReader(inpath, ifilename, igroupname, idataType, iimageNumber, 'fine')
    d.sample.GainArr = d.sample.TxTFrameReader(inpath, ifilename, igroupname, idataType, iimageNumber, 'gain')

    print ('Process the Reset data ...\n')
    idataType = 'Reset'
    iimageNumber = 10-1  # sample frame 10 needs reset frame 9, hence minus 1
    d.reset = Frame(210, 160)
    d.reset.CoarseArr = d.sample.TxTFrameReader(inpath, ifilename, igroupname, idataType, iimageNumber, 'coarse')
    d.reset.FineArr = d.sample.TxTFrameReader(inpath, ifilename, igroupname, idataType, iimageNumber, 'fine')
    d.reset.GainArr = d.sample.TxTFrameReader(inpath, ifilename, igroupname, idataType, iimageNumber, 'gain')


    plt.figure(1)               ####################################################################

    plt.subplot(321)
    plt.imshow(d.sample.GainArr, cmap='gray')
    plt.subplot(323)
    plt.imshow(d.sample.CoarseArr, cmap='gray')
    plt.subplot(325)
    plt.imshow(d.sample.FineArr, cmap='gray')

    plt.subplot(322)
    plt.imshow(d.reset.GainArr, cmap='gray')
    plt.subplot(324)
    plt.imshow(d.reset.CoarseArr, cmap='gray')
    plt.subplot(326)
    plt.imshow(d.reset.FineArr, cmap='gray')

    plt.draw()


    print ('Load the ADC & PTC calibration files ...\n' )
    p=calibParams(7,210,160)
    p.Load_ADC_Calib_H5Files(inpath+'ADC_tr6_W14_14BSI_TChip-40C_bias10f_2014-02-28_14-05_')
    p.Load_ADU2e_Calib_H5Files(inpath+'trPTC8a_W14_14TS1BSI_TChip-40_bias10f_2014_02_28_21-32_PTC7row_ADU2e.h5')

    d.reset.ADUcorrection(p)
    d.sample.ADUcorrection(p)

    plt.figure(2)                   ###############################################################

    plt.subplot(221)
    plt.imshow(d.sample.ADUCorr, cmap='gray')
    plt.subplot(222)
    plt.imshow(d.reset.ADUCorr, cmap='gray')

    plt.draw()

    d.CDSimage()
    plt.subplot(223)
    plt.imshow(d.CDSimg, cmap='gray')
    plt.draw()

    d.ADU2eCor(p)
    plt.subplot(224)
    plt.imshow(d.eImg, cmap='gray')
    plt.draw()

    # check results
    filename = inpath + 'KnifeQuadBPos1CDSed_0_18.h5'
    got_file = h5py.File(filename)
    DESY_CDSimg = np.array(got_file['KnifeQuadBPos1CDSed/10/Sample']).transpose()
    plt.figure(3)                   #################################################################
    plt.subplot(131)
    plt.imshow(DESY_CDSimg.astype('float'), cmap='gray')
    plt.subplot(132)
    plt.imshow(DESY_CDSimg.astype('float')-d.CDSimg, cmap='gray')
    plt.subplot(133)
    plt.imshow(d.CDSimg, cmap='gray')
    plt.draw()

    plt.show()
    
    
    f = h5py.File("mytestfile.hdf5", "w")
    dset = f.create_dataset("mydataset", data = d.sample.ADUCorr)

    f.close()
    
    print d.sample.ADUCorr.shape
    