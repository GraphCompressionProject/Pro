#include "stdafx.h"
#include "properties.hpp"
#include <boost/regex.hpp>
#include <iostream>
#include <ctime>

//#define HARDCORE_DEBUG

namespace webgraph
{

	using namespace std;


	//Constructeur 
	properties::properties()
	{
	}

	//Déstructeur 
	properties::~properties()
	{
	}

	//Charger les propriétés du fichier dans la map "back" 
	void properties::load(std::istream& in) {
		using namespace boost;

		//regex : pour les expressions régulaires 
		regex comment("#.*");
		regex splitter("(.*?)\\s*=\\s*(.*)");
		regex splitterblank("(.*?)\\s*=\\s*$");

		string nextline;

#ifdef HARDCORE_DEBUG
		cerr << "Load called.\n";
#endif

		while (getline(in, nextline)) {
#ifdef HARDCORE_DEBUG
			cerr << "Just fetched line: " << nextline << endl;
#endif

			// throw away comments
			if (regex_match(nextline, comment)) {
				continue;
			}

			smatch what;

			if (regex_match(nextline, what, splitter)) {
				// what[1] contains the key, what[2] contains the value
				back[what[1]] = what[2];
			}
			else if (regex_match(nextline, what, splitterblank)) {
				back[what[1]] = "";
			}
			else {
				//         cerr << "Everything broke for " << nextline << endl;
				abort();
			}
#ifdef HARDCORE_DEBUG      
			cerr << "Loaded property " << what[1] << " = " << back[what[1]] << endl;
#endif
		}
	}

	//fonction inverse : recharger les propriétés sur le fichier 
	void properties::store(std::ostream& out, const std::string& title) {
		using namespace std;

		time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		out << "# " << title << "\n"
			<< "# " << asctime(timeinfo);

		for (map<string, string>::iterator itor = back.begin(); itor != back.end(); ++itor) {
			out << itor->first << "=" << itor->second << "\n";
		}
	}


	void properties::store(std::ostream& out) {
		store(out, "");
	}



}
