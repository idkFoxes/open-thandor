"""Writes globalmap.txt (name, address, sizeof) for every address-defined object in
generated/globals.h and data/recovered.h. The sizes come from the compiler: a generated program
prints sizeof of each object (sizeof does not evaluate the address). Needs cl.exe in PATH
(run from a vcvars32 prompt); without it, only globalmap.c is written."""
import os
import shutil
import subprocess

import common

args = common.parse_arguments(__doc__)
macros = common.address_macros()
source = os.path.join(args.work, 'globalmap.c')
with open(source, 'w') as f:
    f.write('#include <stdio.h>\n#include <thandor/thandor.h>\n\nint main(void)\n{\n')
    for name, (_, addr) in sorted(macros.items(), key=lambda x: x[1][1]):
        f.write('    printf("%%s %%08x %%u\\n", "%s", 0x%08xu, (unsigned)sizeof(%s));\n' % (name, addr, name))
    f.write('    return 0;\n}\n')
cl = shutil.which('cl')
if cl is None:
    raise SystemExit('%s written; cl.exe not found (run from a vcvars32 prompt to compile it)' % source)
exe = os.path.join(args.work, 'globalmap.exe')
subprocess.run([cl, '/nologo', '/w', '/I' + os.path.join(common.REPO, 'include'), source, '/Fe' + exe,
                '/Fo' + os.path.join(args.work, 'globalmap.obj')], check=True, stdout=subprocess.DEVNULL)
out = subprocess.run([exe], capture_output=True, text=True, check=True).stdout
open(os.path.join(args.work, 'globalmap.txt'), 'w').write(out)
print('%d objects -> %s' % (len(out.splitlines()), os.path.join(args.work, 'globalmap.txt')))
