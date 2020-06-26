'''
@author: Angrad.Yang
@software: auto_reformat_code
@time: 2019/4/30 17:22
@desc:
    auto reformat code by tool astyle.
'''

import os

def main():
    asfile="astyle -A1 -c -s4 -xV -S -N -xt4 -xW -w -Y -p -U -k3 -W3 -j  -xf -xh -H --suffix=none"

    get_changed_files_cmd = "git status --short"
    content = os.popen(get_changed_files_cmd).read().strip()
    content = content.split('\n')

    files = list()

    for c in content:
        f = None
        c = c.strip()
        if c.startswith('A'):
            f = c.split(' ')[-1]
        elif c.startswith('M'):
            f = c.split(' ')[-1]
        elif c.startswith('R'):
            f = c.split(' ')[-1]
        if f:
            files.append(f)
    for f in files:
        if f.endswith('.c') or f.endswith('.h') or f.endswith('.cpp'):
            print(f)
            cmd = "%s %s"%(asfile, f)
            os.popen(cmd)

if __name__ == "__main__":
    main()
