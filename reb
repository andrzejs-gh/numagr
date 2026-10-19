#!/usr/bin/env python 

import subprocess
import sys

if __name__ == "__main__":

    build_script = "/home/Andrzej/git/pymod/setup.py"
    cmd = ("build", "--force")

    subprocess.run(
        [sys.executable, build_script, *cmd],
        cwd = "/home/Andrzej/git/pymod",
        check = True,
    )
