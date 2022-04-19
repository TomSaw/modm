# Buffer

Container for 2D Matrix of Pixels with support for plane and palettized
color types and common types of GDDRAM.

## Coordinate System

{0, 0}
   ┌─────────────────────┐
   │ ┌───➤ X             │
   │ │                   │
   │ 🠛 Y                 │
   │                     │
   │                     │
   │                     │
   │                     │
   └─────────────────────┘
               e.g. {127, 63}
## Example

```cpp
#include <modm/graphic/graphic.hpp>

// TODO update this example
buffer = GraphicBuffer<bool, 128, 64>
buffer << "Hello World"
ssd1306.write(buffer);

′′′