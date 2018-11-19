#ifndef  INPUTREADER_H
#define  INPUTREADER_H 
#include "../ElementaryClasses.h"

//CREDIT:: REUSE INPUT READER FROM Hypersplit's original code//
class  InputReader {
public:

	static int dim ;
	static int reps ;

	static std::vector<Rule> ReadFilterFile(const std::string& filename);

	static std::vector<std::vector<unsigned int>> ReadPackets(const std::string& filename);
private:
	static unsigned int inline atoui(const std::string& in);
	static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
	static std::vector<std::string> split(const std::string &s, char delim);

//	static void ReadIPRange(vector<unsigned int>& IPrange, const string& token);
	static void  ReadIPRange(Range1d& IPrange, unsigned int& prefix_length, const std::string& token);
	static void ReadPort(Range1d& Portrange, const std::string& from, const std::string& to);
	static void ReadProtocol(Range1d& Protocol, const std::string& last_token);
	static void ParseRange(Range1d& range, const std::string& text);
	static int ReadFilter(std::vector<std::string>& tokens, std::vector<Rule>& ruleset, unsigned int cost);
	static  void LoadFilters(std::ifstream& fp, std::vector<Rule>& ruleset);
	static std::vector<Rule> ReadFilterFileClassBench(const std::string&  filename);
	static std::vector<Rule> ReadFilterFileMSU(const std::string& filename);

	static const int LOW = 0;
	static const int HIGH = 1;
};

#endif
