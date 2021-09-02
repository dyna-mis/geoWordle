/*
 * json.h
 *
 *  Created on: Nov 24, 2019
 *      Author: ping
 */

#ifndef SRC_JSONM_H_
#define SRC_JSONM_H_
#include <iostream>
#include <fstream>
#include "json.hpp"

struct jsonM{
	nlohmann::json jObject;
	jsonM() {
	};
	void print();
	void debug();
	void output(const char* outfolder,const char* appendix, const char* outFile);
	void addElement(std::string key, std::string value);
	void addElement(std::string key1, jsonM value);
	void addElement(std::string key1, std::string key2, std::string value);
	void addElement(std::string key1, std::string key2, jsonM value);
};




#endif /* SRC_JSONM_H_ */