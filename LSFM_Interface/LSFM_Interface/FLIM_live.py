# -*- coding: utf-8 -*-
"""
Created on Wed May 19 08:51:17 2021

@author: shock
"""

#Libraries
import math
import numpy as np
from hsv_rgb_skimage import hsv2rgb
from scipy.interpolate import interp1d


def get_phase_order(N):
    #complete order of acquired stacks
    p_full = np.uint8(np.zeros(2*N))
    for k in range(N//2):
        p_full[4*k] = k
        p_full[4*k+1] = (k + N//2)%N
        p_full[4*k+2] = (k + N//2)%N
        p_full[4*k+3] = k
    #tab selection
    p_select = np.uint8(np.zeros(N))
    for k in range(len(p_select)):
        p_select[k] = p_full[2*k]
    return p_select

def calc_bmp(N,stack):
    #phase number
    N = np.shape(stack)[0]
    #frequency sampling
    k = np.asarray([j for j in range(N)])
    w_kr = {j : math.cos(2.*math.pi*j/N) for j in k}
    w_ki = {j : math.sin(2.*math.pi*j/N) for j in k}
    for j in k:
        if abs(w_kr[j]) < 1e-6:
            w_kr[j] = 0.
        if abs(w_ki[j]) < 1e-6:
            w_ki[j] = 0.
    #bias
    b = np.mean(stack,axis=0)
    b[b<1e-6] = 1e-6
    #real and imaginary sums
    I_r = np.zeros(np.shape(stack)[1:])
    I_i = np.zeros(np.shape(stack)[1:])
    for j in k:
        I_r += stack[j,:,:]*w_kr[j]
        I_i += stack[j,:,:]*w_ki[j]
    #modulation and phase
    a = 2.*np.sqrt((I_r)**2+(I_i)**2)/N
    m = a/b
    phi = 0.5*np.pi*np.ones(np.shape(I_i))
    phi[abs(I_r)>1e-6] = np.arctan(I_i[abs(I_r)>1e-6]/I_r[abs(I_r)>1e-6])
    return b,m,phi

def normalize(a,I0=0.,I1=1.):
    I_min = np.quantile(a,0.01)
    I_max = np.quantile(a,0.99)
    if I_min == I_max:
        if I_min < 1e-6:
            return a
        else:
            return a/I_min
    else:
        b = (I1-I0)*(a-I_min)/(I_max-I_min)+I0
        b[b<I0] = I0
        b[b>I1] = I1
        return b

def magic_rgb(H,S,V):
    #empty hsv stack
    hsv = np.zeros([*np.shape(H),3])
    #normalize 0 to 1
    [H,S,V] = normalize(H,S,V)
    #add to hsv stack
    for j,I in enumerate([H,S,V]):
        hsv[:,:,j] += I
    #convert to RGB
    rgb = np.uint8(255.*hsv2rgb(hsv))
    return rgb

def polynom_fit_constant(B,M):
    x = np.reshape(B/65535.,-1)
    y = np.reshape(M,-1)
    ex = np.histogram(x,bins=20)[1]
    bx = 0.5*(ex[1:]+ex[:-1])
    by = np.zeros(bx.shape)
    for j in range(len(bx)):
        i = (x>=ex[j])*(x<ex[j+1])
        if np.sum(i) > 0:
            by[j] = np.mean(y[i])
    f = interp1d(bx,by,kind='cubic',bounds_error=False,fill_value='extrapolate')
    M_exp = f(B/65535.)
    M_exp /= np.mean(M_exp)
    return M_exp

def FLIM_rgb(*images,dark=[],M_xy=None):
    #phase ordering (always opposite)
    N = len(images)
    p_order = get_phase_order(N)
    #images to stack in ascending order
    stack = np.zeros([N,*images[0].shape]).astype(images[0].dtype)
    for j,p in enumerate(p_order):
        if len(dark) == 0:
            stack[p,:,:] += images[j]
        else:
            stack[p,:,:] += images[j]-dark[j]
    #calculate bias, modulation, phase
    B,M,P = calc_bmp(N,stack)
    #quick referencing
    if type(M_xy) != type(None):
        M_exp = polynom_fit_constant(B,M)
        M /= (M_xy*M_exp)
    #normalize B and M
    B = normalize(B,I0=0.,I1=1.)
    M = normalize(M,I0=0.8,I1=0.)
    #make hsv
    hsv = np.asarray([M,np.ones(M.shape),B])
    hsv = np.moveaxis(hsv,0,-1)
    #make rgb
    rgb = np.uint8(255.*hsv2rgb(hsv))
    return rgb