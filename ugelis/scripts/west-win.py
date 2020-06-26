# Windows-specific launcher alias for west (west wind?).

import os
import sys

ugelis_base = os.environ['UGELIS_BASE']
sys.path.append(os.path.join(ugelis_base, 'scripts', 'meta'))

from west.main import main  # noqa E402 (silence flake8 warning)

main(sys.argv[1:])
