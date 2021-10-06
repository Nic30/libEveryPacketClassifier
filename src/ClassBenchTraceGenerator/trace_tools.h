// File: trace_tools.h
// David E. Taylor
// Applied Research Laboratory
// Department of Computer Science and Engineering
// Washington University in Saint Louis
// det3@arl.wustl.edu
//
// Functions for generating synthetic trace of headers
//
#pragma once

#include "../ElementaryClasses.h"

std::vector<Packet> header_gen(int d, std::vector<Rule>& filters, float a, float b, int scale);
void RandomCorner(Random & rand, const Rule & filter, Packet& new_hdr, int d);
int MyPareto(Random & rand, float a, float b);
std::vector<Packet> GeneratePacketsFromRuleset(std::vector<Rule>& filters, int num_packets);
