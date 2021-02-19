# FusePDF

FusePDF is an simple application with one purpose, to merge PDF documents.

Requires [Ghostscript](https://www.ghostscript.com/download/gsdnld.html) installed prior to usage.

Copyright (c) 2021 NettStudio AS. All rights reserved.

## Open Source

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

```
git clone https://github.com/nettstudio/fusepdf
cd fusepdf
mkdir build && cd build
qmake CONFIG+=release PREFIX=/usr .. && make
sudo make install
```

Note that FusePDF requires Ghostscript (``gs``) available during runtime (install it through your package manager if not already installed).
