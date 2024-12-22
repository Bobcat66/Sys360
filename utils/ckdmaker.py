import argparse
import os
#A simple script to generate blank CKDDASD image files

def makeCKDDASDimage(
        name : str,
        bins : int,
        cylinders : int,
        tracks : int,
        trackSize : int,
        verbose = False):
    
    filename = f"{name}.ckd"

    if not os.path.exists(filename):
        dasdfile = open(filename,"ab")
    else:
        print(f"ERROR: '{filename}' already exists")
        return
    
    if verbose:
        print(f"{filename} created\nWriting header...")

    #Write header
    headerBuf = bytes(chr(bins//(1<<8))+chr(bins%(1<<8))+chr(cylinders//(1<<8))+chr(cylinders%(1<<8))+chr(tracks//(1<<8))+chr(tracks%(1<<8))+chr(trackSize//(1<<8))+chr(trackSize%(1<<8)),'ascii')
    dasdfile.write(headerBuf)
    if verbose:
        size = bins * cylinders * tracks * trackSize
        print(f"Header written to {filename}\nUnformatted Size: {size} bytes\nGenerating blank data...")
    for bb in range(bins):
        for cc in range(cylinders):
            for tt in range(tracks):
                trackBuf = bytes(trackSize)
                hAddr = chr(0x01)+chr(cc//(1<<8))+chr(cc%(1<<8))+chr(tt//(1<<8))+chr(tt%(1<<8))+chr(0x00)+chr(0x00)
                trackBuf = bytes(hAddr,'ascii') + trackBuf[7:]
                dasdfile.write(trackBuf)
                if verbose:
                    print(f"Track {bb}:{cc}:{tt} written")
    
    if verbose:
        print("CKDDASD Image file generated")
    dasdfile.close()

if __name__ == "__main__":
    makeCKDDASDimage("test",1,5,10,512,True)
    

