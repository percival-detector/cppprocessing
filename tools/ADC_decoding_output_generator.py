import numpy as np

#initialising input lists and decoded arrays in both binary and decimal format
inputDec = [0]
inputBin = [0]

gainDec = [0]
coarseDec = [0]
fineDec = [0]

gainBin = [0]
coarseBin = [0]
fineBin = [0]
# Algorithm for calculating the final output
def output(gain, fine, coarse):
    return (gain) * (fine + coarse * 4)
# opening output file
f = open('../data/ADC_decode_output','w')
# file header
#f.write("This is the output file \n")
# table title
#f.write("{0} \t {1}| \t {2} \t\t {3}| \t {4} \t {5}| \t \t {6} \t {7} \t {8}\n".format('input', 'input', 'output', 'gain', 'gain', 'fine', 'fine', 'coarse', 'coarse'))


for x in range(1, np.power(2, 15)):
    # decoding
    gain = x % 4
    fine = (x >> 2) % np.power(2,8)
    coarse = (x >> 10) % np.power(2,15)
    # pushing entries into lists
    inputDec.append(x)
    inputBin.append("{0:016b}".format(x))
    
    gainDec.append(gain)
    gainBin.append("{0:02b}".format(gain))
    
    fineDec.append(fine)
    fineBin.append("{0:08b}".format(fine))
    
    coarseDec.append(coarse)
    coarseBin.append("{0:05b}".format(coarse))
    # writing to file
    f.write("{0} \t {1}| \t {2} \t {3}| \t {4} \t {5}| \t {6} \t {7} \t {8}\n".format(inputDec[x], inputBin[x], output(gain, fine, coarse), gainDec[x], gainBin[x], fineDec[x], fineBin[x], coarseDec[x], coarseBin[x]))
    #f.write("{0}\t{1}\n".format(inputDec[x], output(gain, fine, coarse)))
# closing file
f.close()



