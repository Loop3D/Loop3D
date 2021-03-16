# Loop Documentation

<!-- TOC seems to break alignment (also note that if TOC is enabled you MUST use level 1 headers) -->
<!-- [TOC] -->

<!-- I would prefer to have an image linked to the loop project website but Doxygen markdown cannot do this -->
<!-- In Doxygen 1.8.16 (released 9/8/2019) it might be possible -->
![Loop Project](loop_inv.png "Loop Project")
[Loop Project Website](https://loop3d.org)

# What is Loop?
The Loop platform is an open source 3D probabilistic geological and geophysical modelling platform, initiated by Geoscience Australia and the OneGeology consortium. The project is funded by Australian territory, State and Federal Geological Surveys, the Australian Research Council and the MinEx Collaborative Research Centre.

# What does the Loop GUI do?
This GUI makes using Loop python packages easier.  Incorporating map2loop and LoopStructural into a basic interface.  This enables the user to select a region
of interest from a simple map interface, run map2loop to extract a geological event history and observation data, and then create a 3D model of the subsurface of that region
using LoopStructural.  An area of 100x100km takes a little under 10 minutes from start to finish.

Additionally the GUI allows the user to correct data or remove extracted data that is believed to be erroneous and change options for running the modelling scripts.
This enables the user to quickly try different conceptual models of how the region geologically formed.

# Licencing
The Loop3D GUI repository is under the LGPL license 

