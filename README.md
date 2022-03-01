# FusePDF

[![GitHub](https://img.shields.io/github/license/nettstudio/fusepdf)](https://github.com/nettstudio/fusepdf/blob/main/COPYING) [![C/C++ CI](https://github.com/nettstudio/fusepdf/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/nettstudio/fusepdf/actions/workflows/c-cpp.yml) [![GitHub issues](https://img.shields.io/github/issues/nettstudio/fusepdf)](https://github.com/nettstudio/fusepdf/issues)

FusePDF is a simple cross-platform application used for merging, splitting and exporting PDF pages and documents.

## License

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

Copyright (c) 2021, 2022 [NettStudio AS](https://nettstudio.no). All rights reserved.

[![NettStudio](https://raw.githubusercontent.com/nettstudio/fusepdf/main/assets/nettstudio.png)](https://nettstudio.no)

## Build (on macOS/Linux/BSD)

FusePDF requires Qt5+ (Core/Gui/Widgets/Concurrent) to build, and Ghostscript (``gs``) available during runtime.

```
git clone https://github.com/nettstudio/fusepdf
cd fusepdf
git submodule update -i
mkdir build && cd build
qmake CONFIG+=release PREFIX=/usr .. && make
```
