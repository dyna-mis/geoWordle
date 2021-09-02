## <img src="dynamis_icon.gif" width="100"> Dynamic Maximum/Maximal Independent Set Solver
for unit-height rectangle intersection graphs

Github Page: https://dyna-mis.github.io/dynaMIS/
# Requirements
<ol>
<li>CGAL (>= 4.11.2)</li>
<li>  g++ </li>
</ol>
# Description
Our Framework contains 5 algorithms.
* ors(rs) : a dynamic MIS algorithm based on orthogonal range searching (for unit squares)
* graph(cg): a graph-based dynamic MIS algorithm
* grid(sl): a grid-based 4-approximation algorithm （for unit squares）
* gridK(slk): The group-shifting based algorithm. Need a extra parameter -k (for unit squares)
* line(arrow): stabbing-line based 2-approximation algorithm
##<embed src="rectilinear-polygon.pdf" width="800px" height="2100px" />
Additionally, our framework supports the greedy optimization with the maximal independent set solver,i.e., grid, gridk, and line.  The greedy versions drastically improve the solution
 quality, but typically at the cost of higher runtimes. 

# Installation
Compile the source with the Debug/Makefile file.
# Usage
./dynaMIS  <instance> [options]


--output, -o : output the solution</br>
--help, -h : output this help</br>
--algorithm, -a : algorithm in use, please using the abbreviations (see description above). To adding greedy optimization, just add 'g' to the algorithm name, e.g., 'garrow'. </br>
--sigma, -s : width of a square</br>



# Acknowledgments
DynaMis is part of the project ["Human-Centered Algorithm Engineering: Graph and Map Visualization"](https://www.ac.tuwien.ac.at/research/humalgo/) supported by the Austrian Science Fund (FWF) under Grant P31119.
# Reference
More details see our paper "An Algorithmic Study of Fully Dynamic Independent Sets for Map Labeling"([ARXIV LINK](https://arxiv.org/abs/2002.07611))

