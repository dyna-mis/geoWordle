#include "wmis_solver.h"
string maxHS_PATH = "/home1/guangping/mis_solvers/maxhs ";
string kamis_reduce_PATH = "/home1/guangping/mis_solvers/weighted_branch_reduce ";
string kamis_local_PATH = "/home1/guangping/mis_solvers/weighted_local_search ";

void wmis_solver::set(problem& P, size_t boxN, size_t N, int gurobi_focus, string solver_id,
    const std::vector<size_t>* boxNumbers, bool M_flag_p) {
    solver_index = N;
    file_name = P.get_file_name();
    solver::set(P);
    solution_indices.reserve(pointNumber);



};
int wmis_solver::set_conflict_graph() {
    int count = 0;
    size_t temp = candidates_set.size();
    cg = Graph(temp);
    for (size_t i = 0; i < temp; i++) {
        for (size_t j = i+1; j < temp; j++) {
            if (!get<0>(candidates_set[i]).is_disjoint(get<0>(candidates_set[j]))) {
                cg.addEdge(i, j);
                count++;
            }
        
        }
    
    }
    return count;
}

void wmis_solver::setSolution(problem& P) {
    assert(solution.size() == 0);
    //!DEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
    debugSolution_with_auxilaries(P, false);
#endif

    //!DEBUG---------------debug------------------------
#ifdef GEOWORDLE_PRO_RATIO
    for (const auto& b : solution_indices) {
        const colored_box& e = get<0>(candidates_set[b]);

        auto text = P.texts[e.c];
        auto got = P.text_ratios_map[e.c];
        solution.push_back(colored_box(
            e.xmin()
            , e.xmax()
            , e.ymin()
            , e.ymax()
            , P.texts[e.c], get<0>(got), get<1>(got), get<2>(got), e.c)
        );
    }
    return;
#endif


#ifdef GEOWORDLE_PRO_Font_Size
    for (const auto& b : solution_indices) {
        const colored_box& e = get<0>(candidates_set[b]);

        auto text = P.texts[e.c];
        auto got = P.text_ratios_map[e.c];
        solution.push_back(colored_box(
            e.xmin()
            , e.xmax()
            , e.ymin()
            , e.ymax()
            , P.texts[e.c], e.c)
        );
    }
    return;
#endif


#ifdef GEOWORDLE_PRO
    for (const auto& b : solution_indices) {
        const colored_box& e = get<0>(candidates_set[b]);

        auto text = P.texts[e.c];
        solution.push_back(colored_box(
            e.xmin() - raw_epsilon_x
            , e.xmax() + raw_epsilon_x
            , e.ymin() - raw_epsilon_y
            , e.ymax() + raw_epsilon_y
            , P.texts[e.c], e.c)
        );
    }
    return;

#endif

    for (const auto& b : solution_indices) {
        const colored_box& e = get<0>(candidates_set[b]);
        solution.push_back(colored_box(
            e.xmin() - raw_epsilon_x
            , e.xmax() + raw_epsilon_x
            , e.ymin() - raw_epsilon_y
            , e.ymax() + raw_epsilon_y
            , P.texts[e.c], e.c)
        );
    }



}
void wmis_solver::write_solver_input_kamis() {
    ofstream outdata;
    string filename = file_name + "_kamis.tmp";
    double numV = cg.get_vertices_size();
    outdata.open(filename.c_str());
    if (!outdata) {
        // file couldn't be opened
        cerr << "Error:write_solver_input_kamis file could not be opened" << endl;
        exit(1);
    }
    int eSize = cg.get_edges_size();
    outdata << numV << " " << eSize << " " << 10 << endl;
    for (int i = 0; i < numV; i++) {
        outdata << 2 * get<1>(candidates_set[i]) - 1 << " ";
        std::vector<double> output(cg.adList[i].size());
        std::copy(cg.adList[i].begin(), cg.adList[i].end(), output.begin());
        sort(output.begin(), output.end());
        for (const auto& q : output) {
            outdata << q + 1 << " ";
        }
        outdata << std::endl;
    }
    outdata.close();
}
void wmis_solver::write_solver_input_maxHS(){
    ofstream outdata;
    int count = 0;
    string filename = tmp_dictionary + "/"+ get_file_name(file_name) + getAppendix()+"_wmis.tmp";
    int numV = cg.get_vertices_size();
#ifndef GEOWORDLE_PRO_Font_Size
    int top = 2 *pointNumber*candidates_set.size();
#else
    int top = 2 * pointNumber* pointNumber*candidates_set.size();
#endif
    outdata.open(filename.c_str());
    if (!outdata) {
        // file couldn't be opened
        cerr << filename<< " Error: (write_solver_input_maxHS) file could not be opened" << endl;
        exit(1);
    }
    outdata << "c comments Partial Max - SAT " << filename << endl;
    int eSize = cg.get_edges_size();
    outdata << fixed << std::setprecision(0)<< "p wcnf " << numV  << " " << eSize + numV <<" "<< top<< endl;
    for (int i = 0; i < numV; i++) {
        std::vector<double> output(cg.adList[i].size());
        std::copy(cg.adList[i].begin(), cg.adList[i].end(), output.begin());
        sort(output.begin(), output.end());
        for (const auto& q : output) {
            if (i < q) {
                count += 1;
                outdata << top << " " << -(i + 1) << " " << -(q + 1) << " " << 0 << endl;
            }
        }
    }
    for (unsigned int i = 0; i < numV; i++) {
#ifndef GEOWORDLE_PRO_Font_Size
        outdata << 2*get<1>(candidates_set[i])-1 << " " << i + 1 << " " << 0 << endl;
#else
        outdata << 2 * get<1>(candidates_set[i])*pointNumber - 1 << " " << i + 1 << " " << 0 << endl;
#endif
    }
    outdata<<flush;
    outdata.close();
}




SolveStatus wmis_solver::solve(problem& P, jsonM& measures, size_t N) {
    set_candidate_set(P, measures);
    int count = set_conflict_graph();
    measures.addElement("info", "number_conflcts", to_string(count));
    outputMeasure("_info.json");
#ifdef GEOWORDLE_PRINT
    printSolution_with_auxilaries();
#endif

    if (solver_index == 3) {
        getMAXHS_maxHS();
    }
    else {
        getMAXHS_kamis();
    }
    setSolution(P);
    return SolveStatus::Optimal;

};
void wmis_solver::getMAXHS_kamis() {
    write_solver_input_kamis();
    string kamis_PATH;
    if (solver_index == 0) 
    {
        kamis_PATH = kamis_local_PATH;
    }
    else {
        kamis_PATH = kamis_reduce_PATH;
    }
    string o = kamis_PATH + file_name + "_kamis.tmp --output=" + file_name + "_kamis_solution.tmp";
    if(EXIT_SUCCESS != system(o.c_str()))
    {
        assert(false);
        exit(1);
    }
    //system("/home1/guangping/dynaMIS/maxhs /home1/guangping/dynaMIS/dimacs.txt >/home1/guangping/dynaMIS/b.txt");
    readWCNF_kamis();
}
void wmis_solver::getMAXHS_maxHS() {
    write_solver_input_maxHS();
    string o = maxHS_PATH + tmp_dictionary + "/" + get_file_name(file_name)
        + getAppendix() + "_wmis.tmp>" 
        + tmp_dictionary + "/" + get_file_name(file_name) 
        + getAppendix()+"_wmis_solution.tmp";
    std::cout << o << std::endl;
    if (EXIT_SUCCESS != system(o.c_str()))
    {
        assert(false);
        exit(1);
    }
    // remove the datas
    string f = tmp_dictionary +"/"+ get_file_name(file_name)
        + getAppendix() + "_wmis.tmp";
    if (remove(f.c_str()) != 0)
        perror("Error deleting instance file");

    //system("/home1/guangping/dynaMIS/maxhs /home1/guangping/dynaMIS/dimacs.txt >/home1/guangping/dynaMIS/b.txt");
    readWCNF_maxHS();
}

void wmis_solver::readWCNF_kamis() {
    string WCNF = file_name + "_kamis_solution.tmp";
    ifstream indata;
    indata.open(WCNF.c_str());
    if (!indata) {
        // file couldn't be opened
        cerr << "Error: file could not be opened" << endl;
        exit(1);
    }
    string line;
    char head;
    size_t i = 0;
    while (getline(indata, line)) {
        if (line.empty()) break;
        head = line.at(0);
        if (head == '1') {
            solution_indices.push_back(i);
        }
        i++;
    }
}
void wmis_solver::readWCNF_maxHS() {
    string WCNF = tmp_dictionary + "/" + get_file_name(file_name) + getAppendix()+"_wmis_solution.tmp";
    ifstream indata;
    indata.open(WCNF.c_str());
    if (!indata) {
        // file couldn't be opened
        cerr << "Error: readWCNF_maxHS file could not be opened" << endl;
        cerr << "Error:"<< WCNF<< endl;
        exit(1);
    }
    string line;
    char head;
    while (getline(indata, line)) {
        if (line.empty()) continue;
        head = line.at(0);
        if (head == 'v') {
            break;
        }
    }
    int i;
    char* str = strdup(line.c_str());
    const char s[2] = " ";
    char* token = strtok(str, s);
    token = strtok(NULL, s);
    while (token != NULL) {
        i = atoi(token);
        if (i > 0) {
            solution_indices.push_back(size_t(i-1));
        }
        token = strtok(NULL, s);
    }
    indata.close();
    
    // remove the datas
    if (remove(WCNF.c_str()) != 0)
        perror("Error deleting file");
    else
        puts("File successfully deleted");
}



//?PRINT+++++++++++++++PRINT+++++++++++++++++++++++++
#ifdef GEOWORDLE_PRINT
	//+++++++++++++++print+++++++++++++++++++++++++
void wmis_solver::printSolution_with_auxilaries() {
    std::cout << "candidates set: " << std::endl;
    for (auto& c : candidates_set) {
        std::cout << "<";
        get<0>(c).print();
        std::cout << get<1>(c) << ",  ";
        std::cout << get<2>(c) << "> " << std::endl;
    }
    cg.printGraph();
};
	//---------------print-------------------------
#endif
//?PRINT---------------PRINT-------------------------

//xDEBUG+++++++++++++++debug++++++++++++++++++++++++
#ifndef NDEBUG
	// test if the instance is valid
void wmis_solver::debugSetting(problem& P) {



};

	
void wmis_solver::debugSolution_fullCover_with_auxilaries(problem& P) {





};
void wmis_solver::debugSolution_disjoint_with_auxilaries(bool colored) {





};
#endif
	//xDEBUG---------------debug------------------------