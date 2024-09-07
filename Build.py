# Runs premake to build the solution and all projects within.

import os
import sys
import subprocess

if __name__ == "__main__":
    subprocess.run(("./Premake/premake5.exe", "vs2019"))

