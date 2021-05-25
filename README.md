# FusePDF

FusePDF is a simple application used for merging various PDF documents into one.

Copyright (c) 2021 NettStudio AS. All rights reserved.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

## Build

FusePDF requires Qt5+ (Core/Gui/Widgets/Concurrent) to build, and Ghostscript (``gs``) available during runtime.

```
git clone https://github.com/nettstudio/fusepdf
cd fusepdf
mkdir build && cd build
qmake CONFIG+=release PREFIX=/usr .. && make
sudo make install
```
