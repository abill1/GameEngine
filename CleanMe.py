# This Python script is used to remove files and directories
# created after a build.  

import os
import shutil

def DeleteDirectory(root, dirName):
    Path = os.path.join(root, dirName)
    if os.path.exists(Path):
        print('Deleting Directory:', Path)
        shutil.rmtree(Path)

if __name__ == "__main__":
    print("Executing CleanMe.py ...")
    path = os.getcwd()
    DeleteDirectory(path, 'build')
    DeleteDirectory(path, '.vs')
    print("CleanMe.py complete.")

