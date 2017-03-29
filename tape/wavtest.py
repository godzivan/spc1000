#!/usr/bin/python
# wavtest.py
#
# Author : Miso Kim (http://github.com/meesokim)
# Copyright (C) 2017
#
from __future__ import print_function

def normalized(a, axis=-1, order=2):
    l2 = np.atleast_1d(np.linalg.norm(a, order, axis))
    l2[l2==0] = 1
    return a / np.expand_dims(l2, axis)

if __name__ == '__main__':
    import wave
    import sys
    import pylab
    import numpy as np
    import struct
    if len(sys.argv) < 2:
        print("Usage: %s infile" % sys.argv[0])
        raise SystemExit(1)

    f = wave.open(sys.argv[1])
    channels = f.getnchannels()
    samples = f.getsampwidth()
    frames = f.getnframes()
    print ("channels =", samples)
    print ("sample width=", samples)
    print ("frames =", frames)
    nframes = 0
    if len(sys.argv) > 2:
        f.setpos(int(sys.argv[2]))
        nframes = int(sys.argv[2])/samples
    rev = 0
    st = np.zeros(100).astype(int)
    n = 0
    length = 0
    while True:   
        draw = False
        if rev > 0:
            f.setpos(f.tell()-rev)     
        rawdata = bytearray(f.readframes(2000))
        nframes = nframes + len(rawdata)/samples
        if nframes > frames:
            break
        msdata = np.array(struct.unpack('h'*(len(rawdata)/2), rawdata)) /100.0
        if channels == 2:
            lframes = msdata[::2]
            rframes = msdata[1::2]
            msdata = lframes/2 + rframes/2
        for i in range(0,len(msdata)):
            msdata[i] = 0 if abs(msdata[i]) < 2 else msdata[i]
        a = msdata[1:-1]-msdata[0:-2]
        r = np.average(np.abs(msdata))
        if r < 10.:
            continue
       # b = np.where(np.abs(np.sign(msdata[1:-1]-0.1)-np.sign(msdata[0:-2]+0.1))>1)[0]+1
        e = np.sign(msdata[1:-1]*msdata[0:-2])
        b = np.where(np.diff(np.signbit(msdata)))[0]
        c = np.zeros(len(b)).astype(int)
        k = 0
        for i in range(1,len(b)):
           c[k] = np.argmax(np.absolute(msdata[b[i-1]:b[i]])) + b[i-1]
           k = k + 1
        m = normalized(msdata,0)[0]
        c1 = c[1:-1]-c[0:-2]
        c2 = np.zeros(len(b)).astype(int)
        k = 0
        k = c[0]
        c2[0] = c[0]
        j = 1
        for i in range(1, len(c1)):
            if c1[i] > 4 and c1[i] < 60:
                c2[j] = c[i]
                j = j + 1
        c2[j] = c[len(c1)]
        c2 = np.resize(c2, j+1)
        c = c2
        #if any(b1<6):
        #print ("*",b[1:-1]-b[0:-2])
        #print ("*",e)
        #c = c2        
        #print (m)
        
        d = c[2:-1:2]-c[1:-2:2]
        d2 = c[2:-1:2]-c[0:-3:2]
        d3 = d+d2
        e = msdata[c[2:-1:2]]-msdata[c[1:-2:2]]
        g = d>12
        if len(c) > 3:
            for i in range(0,len(d)):
                g[i] = False if d2[i] < 30 and d[i] >12 else g[i]
            if any(d==13) or any(d2==14) or any(d2==15):
                draw = False
            if any(g) == True:
                draw = False
            if msdata[c[-2]] - msdata[c[-3]] < 0:
                endl = c[-3]
            else:
                endl = c[-4]
            if any(d<5):
                draw = False
            rev = len(msdata)-endl
            for i in d3:
                if i < 100:
                    st[i] = st[i] + 1
            #print (d)
            #print (e)
            s = ''.join(chr(i+ord('0')) for i in g*1)
            if any(g) == True:
                print (s,end='')
                length = length + len(g)
                if length > 7439-80:
                    draw = False
            sys.stdout.flush()
            if draw == True:
                #print (b[1:-1]-b[0:-2])
                _, ax = pylab.subplots(1, 1, figsize=(16, 6)) 
                ax.plot(msdata)
                ax.plot(b, msdata[b], '1')
                ax.plot(c, msdata[c], '*')
                ind = c[2:-1:2]
                for i in range(0,len(ind)):
                    ax.text(ind[i]-d[i]/2, 0, '%d' % g[i], ha='center', va='bottom')
                    ax.text(ind[i]-d[i]/2, -50, '%d' % d[i], ha='center', va='bottom')
                    ax.text(ind[i]-d2[i]/2, -100, '%d' % d2[i], ha='center', va='bottom')
                ax.plot((c[2:-1:2],c[1:-2:2]),(0,0),'k-')
                ax.plot((endl,endl),(100,-100),'c-')
                pylab.show()
    print (st)