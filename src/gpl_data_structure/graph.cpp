/*
 * graph.cpp
 *
 *  Created on: Sep 24, 2019
 *      Author: guangping
 */
#include "graph.h"
#include <float.h>
#include <string.h>
Graph::Graph() {

}
Graph::Graph(int n) {
    numV = n;
    adList = vector<edgeList>(n, edgeList());
}

size_t Graph::get_edges_size()
{
    size_t eSize = 0;
    for (unsigned int i = 0; i < numV; i++) {
        eSize += adList[i].size();
    }
    return eSize / 2;
}

size_t Graph::get_vertices_size() {
    return numV;
}
void Graph::addEdge(unsigned int v1, unsigned int v2) {
    //assert(v1< numV);
    //assert(v2 < numV);
    adList[v1].insert(v2);
    adList[v2].insert(v1);
};
// check if one edge between two vertices exists
// v1: vertexID
// v2: vertexID
bool Graph::containEdge(unsigned int v1, unsigned int v2) {
    return (adList[v1].count(v2) != 0 && adList[v2].count(v1) != 0);
};
// check vertexID<< numV
// check if edge bidirected (in an undirected graph)
#ifndef NDEBUG
void Graph::debugGraph() {
    edgeList::iterator it;
    for (unsigned int i = 0; i < numV; i++) {
        it = adList[i].begin();
        while (it != adList[i].end()) {
            //undirected graph (u,v)->(v,u)
            assert(containEdge(*it,i));
            //index < numV
            assert(*it <= numV);
            it++;
        }
    }
};
#endif
//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
    //+++++++++++++++print+++++++++++++++++++++++++
// print adjecency list
void Graph::printGraph() {
    int eSize = 0;
    for (unsigned int i = 0; i < numV; i++) {
        eSize += adList[i].size();
    }
    std::cout << "number of conflicts: " << eSize / 2 << endl;
    for (unsigned int i = 0; i < numV; ++i) {
        cout << i << ": ";
        for (auto itr = adList[i].begin(); itr != adList[i].end(); ++itr) {
            cout << *itr << " ";
        }
        cout << endl;
    }
}
//---------------print-------------------------
#endif
//?PRINT---------------PRINT-------------------------

void Graph::deleteEdge(unsigned int v1, unsigned int v2) {
    //assert(v1< numV);
    //assert(v2 < numV);
    adList[v1].erase(v2);
    adList[v2].erase(v1);

};
void Graph::addVertex(unsigned int index, vector<int>& neighbors) {
    //assert(index == numV);
    numV++;
    adList[index].clear();
    for (auto n : neighbors)
        addEdge(index, n);
};
void Graph::deleteVertex(unsigned int index, unsigned int replace) {
    for (auto n : adList[index]) {
        adList[n].erase(index);
    }
    if (index == replace) {
        //assert(replace == numV-1);
        numV--;
        return;
    }
    for (auto n : adList[replace]) {
        adList[n].erase(replace);
        adList[n].insert(index);
    }
    adList[index] = adList[replace];
    numV--;
};
int main_static(int argc, char* argv[]) {
    Graph graph(3);
    graph.addEdge(0, 2);
    graph.addEdge(1, 2);
#ifdef GEOWORDLE_PRINT
    graph.printGraph();
#endif
    //cout<< "POINT A"<<endl;
#ifndef NDEBUG
    graph.debugGraph();
#endif
    //cout<< "POINT B"<<endl;

    /*
    vector<short> maxHS(3, 0);
    graph.outputDIMACS("dimacs_set.txt");
    //cout<< "POINT C"<<endl;
    system("./maxhs dimacs_set.txt >b.txt");
    //cout<< "POINT D"<<endl;
    graph.readWCNF(maxHS, "b.txt");
    cout << "MAXHS" << endl;
    for (const auto elem : maxHS) {
        cout << elem << " ";
    }
    cout << endl;
*/
    return 0;


}
int main_dynamic(int argc, char* argv[]) {
    Graph graph(3);
    graph.addEdge(0, 2);
    graph.addEdge(1, 2);
    graph.deleteVertex(1, 2);
    vector<int> neighbors{ 0,1 };
    graph.addVertex(2, neighbors);
#ifdef GEOWORDLE_PRINT
    graph.printGraph();
#endif
#ifndef NDEBUG
    graph.debugGraph();
#endif
    cout << endl;
    return 0;
}