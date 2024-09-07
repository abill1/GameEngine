# This Python script is used to remove files and directories
# created after a build. Additionally, this script removes 
# all .sln and .proj files.

import os
import shutil

def DeleteDirectory(root, dirName):
    Path = os.path.join(root, dirName)
    if os.path.exists(Path):
        print('Deleting Directory:', Path)
        shutil.rmtree(Path)

def RemoveFilesByExtension(file, extension):
	if file.endswith(extension):
		os.remove(file)

def DeepClean(root):
	subdirs = os.listdir(root)
	for file in subdirs:
		if os.path.isdir(file):
			DeepClean(file)
		RemoveFilesByExtension(os.path.join(root, file), ".vcxproj")
		RemoveFilesByExtension(os.path.join(root, file), ".vcxproj.user")
		RemoveFilesByExtension(os.path.join(root, file), ".sln")
	
if __name__ == "__main__":
	print("Executing DeepClean.py ...")
	path = os.getcwd()
	DeleteDirectory(path, 'build')
	DeleteDirectory(path, '.vs')
	DeepClean(path)
	print("DeepClean.py complete.")
