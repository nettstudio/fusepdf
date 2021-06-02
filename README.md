# FusePDF

[![GitHub](https://img.shields.io/github/license/nettstudio/fusepdf)](https://github.com/nettstudio/fusepdf/blob/main/COPYING) [![C/C++ CI](https://github.com/nettstudio/fusepdf/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/nettstudio/fusepdf/actions/workflows/c-cpp.yml) [![GitHub issues](https://img.shields.io/github/issues/nettstudio/fusepdf)](https://github.com/nettstudio/fusepdf/issues)

FusePDF is a simple cross-platform *(Windows/macOS/Linux)* application used for merging, splitting and exporting *(PNG/TIFF RGB/CMYK)* PDF pages and documents.

![screenshot1](https://github.com/nettstudio/fusepdf/raw/main/assets/fusepdf-screenshot.png "FusePDF screenshot main interface")

![screenshot2](https://github.com/nettstudio/fusepdf/raw/main/assets/fusepdf-screenshot-2.png "FusePDF screenshot pages in document")

## License
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

Copyright (c) 2021 [NettStudio AS](https://nettstudio.no). All rights reserved.

[![NettStudio](https://raw.githubusercontent.com/nettstudio/fusepdf/main/assets/nettstudio.png)](https://nettstudio.no)

## Build

FusePDF requires Qt5+ (Core/Gui/Widgets/Concurrent) to build, and Ghostscript (``gs``) available during runtime.

```
git clone https://github.com/nettstudio/fusepdf
cd fusepdf
mkdir build && cd build
qmake CONFIG+=release PREFIX=/usr .. && make
sudo make install
```
