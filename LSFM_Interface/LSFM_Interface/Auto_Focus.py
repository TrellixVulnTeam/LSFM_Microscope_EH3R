from glob import glob
import os
import numpy as np
import tifffile as tf
import matplotlib.pyplot as plt
from random import sample

#sub-function: evaluate image file
def get_focus(file):
    #read image
    im = tf.imread(file)
    #compute integral of radial amplitudes
    return ira(im)

#sub-function: integral of radial amplitudes
def ira(im):
    #complex fft
    G = np.fft.fft2(im)
    #cartesian coordinates
    a = np.real(G)
    b = np.imag(G)
    #amplitudes
    r = np.hypot(a,b)
    #frequencies
    #1d frequency vectors
    fx = np.fft.fftfreq(np.shape(im)[0])
    fy = np.fft.fftfreq(np.shape(im)[1])
    #create freqeuncy map
    FY,FX = np.meshgrid(fy,fx)
    f = np.hypot(FY,FX)
    #frequency sampling
    Nr = min(np.shape(im))//2
    #first and last frequency of interest
    f_1 = min(np.max(f[0, :]), np.max(f[:, 0]))
    f = np.reshape(f,-1)
    f_u = np.unique(f)
    f_0 = f_u[1]
    df = (f_1-f_0)/Nr
    #frequency vectors
    f_d = np.linspace(f_0,f_1-df,Nr)
    f_u = np.linspace(f_0+df,f_1,Nr)
    #amplitude vector
    r_v = np.zeros(Nr)
    r = np.reshape(r,-1)
    for j in range(Nr):
        r_v[j] += np.mean(r[(f>=f_d[j])*(f<f_u[j])])
    #calculate integral
    Ir = np.sum(np.log(r_v))
    Ir += np.log(np.mean(im))
    return Ir

#sub-function: estimate new position
def find_z(z,f):
    f_max = sorted(f)[-4:]
    z_max = z[f.argsort()[::-1][:4]]
    c2 = np.polyfit(z_max,f_max,2)
    z_1 = int(-1.*c2[1]/(2*c2[0]))
    return z_1

#generator-function: estimate new position
def estimate_z(foc_dict,flist):
    #quadratic fit to find new optimum estimation
    x = np.asarray(list(foc_dict.keys()))
    y = np.asarray(list(foc_dict.values()))
    z_new = find_z(x,y)
    #return value if already evaluated
    if z_new in x:
        #yield same value
        z_opt = x[np.argmax(y)]
        neighbours = [z for z in np.arange(z_opt-2,z_opt+3,1) if z not in x]
        if len(neighbours) == 0:
            yield z_opt
            yield -1
        else:
            z_new = min(neighbours)
            yield z_new
            foc_dict[z_new] = get_focus(flist[z_new])
            yield from estimate_z(foc_dict,flist)
    #lower bound case (just for simulation)
    elif z_new < 0:
        yield z_new
        z_new = max([z for z in range(len(flist)) if z not in x])
        foc_dict[z_new] = get_focus(flist[z_new])
        yield from estimate_z(foc_dict,flist)
    #upper bound case (just for simulation)
    elif z_new >= len(flist):
        yield z_new
        z_new = min([z for z in range(len(flist)) if z not in x])
        foc_dict[z_new] = get_focus(flist[z_new])
        yield from estimate_z(foc_dict,flist)
    else:
        #yield new optimum estimation
        yield z_new
        #append new optimum estimation to focus point dictionary
        foc_dict[z_new] = get_focus(flist[z_new])
        #restart calculation
        yield from estimate_z(foc_dict,flist)
#
# #directory containing images
# d = os.path.join('E:\\','PROJECTS','data_eval_python','data_in','auto_focus_test','lumilass_1_split')
#
# #find image files
# fin = glob(os.path.join(d,'*.tif'))
#
# #initial evaluation points
# #z_pos = np.arange(0,10*len(fin)//10,10)
# z_pos = sample(range(len(fin)),5)
# focus = {z : get_focus(fin[z]) for z in z_pos}
#
# #generator function
# s = estimate_z(focus,fin)
#
# #find focus
# while True:
#     zf = next(s)
#     if zf == -1:
#         break
#     else:
#         print(zf)