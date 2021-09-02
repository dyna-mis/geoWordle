/*
 * graph.h
 *
 *  Created on: Sep 24, 2019
 *  Author: guangping
 *  Self-defined graph representation.
 *  Considering the extern algorithms we choose may be sensitive to the size of verices, we use this way.
 * 
 * 
 * adapted 2021
 * remove the dynamic part
 * add vertex weights
 */
#pragma once
//#include <assert.h>
#include <unordered_set>
#include <iostream>
#include<fstream>
#include <unordered_set>
#include <numeric>
#include <string.h>
#include <stdlib.h>
#include "miscellaneous.h"
using namespace std;
typedef unordered_set<unsigned int> edgeList;
class Graph {
public:
    friend class wmis_solver;
    //friend class colored_mincut_tree;
    Graph();
    Graph(int n);
    void addEdge(unsigned int source, unsigned int target);
    bool containEdge(unsigned int source, unsigned int target);
    //dynamic
    void deleteEdge(unsigned int source, unsigned int target);
    void addVertex(unsigned int index, vector<int>& neighbors);
    void deleteVertex(unsigned int index, unsigned int replace);
    size_t get_edges_size();
    size_t get_vertices_size();



//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
    void printGraph();
#endif
//?PRINT---------------PRINT-------------------------

//!DEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
    void debugGraph();
#endif
//!DEBUG---------------debug------------------------
protected:
    size_t numV;
    vector<edgeList> adList;
};
