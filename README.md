# GROW A VILLAGE

**"GROW A VILLAGE"** [version IN-DEV]\
Simple digital clone of a "[CATAN](https://en.wikipedia.org/wiki/Catan)" board game or a digital counterpart "[colonist.io](https://colonist.io/)" with a few twists. \
The rules are simple: roll the dice
gather and manage resources, buy and build roads and houses. 🎲 \
The goal is to claim the victory by developing the greatest village on the map. 👑

## Powered by

  [![Allegro Game Programming Library logo](https://liballeg.org/images/logo.png)](https://liballeg.org/)

## Maps File Format
Map is defined in a CSV (Comma Separated Values) file named "map.csv". \
Structure of this file consists of:
first row (header): | `MAP_ROWS`,`MAP_COLS`
--------------------|---------------------------------------------
odd rows:           | `TYPE` x `MAP_COLS` (separated by ",")
odd rows:           | `TYPE` x (`MAP_COLS` - 1) (separated by ",")

where `TYPE` might be:
keycode | field type
--------|-----------
"R"     | *RANDOM*
"W"     | *WATER*
"0"     | *DESERT*
"1"     | WOOD
"2"     | BRICKS
"3"     | WHEAT
"4"     | ORE
"5"     | SHEEP

## Acknowledgments

  - **[CATAN](https://en.wikipedia.org/wiki/Catan)** — original board game developed by Klaus Teuber
  - **[COLONIST.IO](https://colonist.io/)** — digital counterpart

### <!-- Footer -->
**Poznan University of Technology 🎓** \
*Low-Level Programming in C* \
Project developed with ❤ by **Konrad Ceglarski** [@xKond3i](https://github.com/xkond3i)
