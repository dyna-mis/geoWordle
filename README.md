Worbel: Aggregating Point Labels into Word Clouds
for unit-height rectangle intersection graphs

Github Page: https://dyna-mis.github.io/geoWordle/
# Requirements
<ol>
<li> MaxHS solver : http://www.maxhs.org/ </li>
<li>  g++ </li>
</ol>
# Description
Our Framework contains two algorithms.
* exact(exact) : a worbel generation algorithm based on SAT solver
* greedy(greedy): a greedy worbel generation algorithm
##<embed src="rectilinear-polygon.pdf" width="800px" height="2100px" />
Additionally, more geometric approaches will be included soon in our framework.

# Installation
Compile the source with the Debug/Makefile file.
# Usage
./geoWordle*  <instance> [options]


--output, -o : output the solution</br>
--help, -h : output this help</br>
-l and -u : lower and upper bound of the width-height ratio
-f : minimum font size
-m : misscover tolerance ratio 
--algorithm, -a : algorithm in use, please using the abbreviations (see description above).</br>
--sigma, -s : width of a square</br>



# Acknowledgments
DynaMis is part of the project ["Human-Centered Algorithm Engineering: Graph and Map Visualization"](https://www.ac.tuwien.ac.at/research/humalgo/) supported by the Austrian Science Fund (FWF) under Grant P31119, P 31336 and Y1329.
# Reference
More details see our paper "Worbel: Aggregating Point Labels into Word Clouds"([ARXIV LINK](comming soon))

