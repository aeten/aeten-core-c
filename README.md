Æten C core library
===================

Object oriented C library.

Headers and the corresponding C skeletons implemention can be generated from UML class diagramm.
The generation supports XMI (tested from PlantUML xmi:star output).

The build system is under construction and needs some improvements:

    ./configure -b ninja # Print error before init bellow
    ninja init
    ninja test
