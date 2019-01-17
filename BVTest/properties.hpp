#ifndef PROPERTIES_HPP_
#define PROPERTIES_HPP_

#include <fstream>
#include <string>
#include <map>

namespace webgraph
{

	class properties
	{
	private:
		std::map<std::string, std::string> back;

	public:
		//constructeur 
		properties();

		//déstructeur 
		virtual ~properties();

		//Charger les propriétés du fichier dans la map "back"
		void load(std::istream& in);

		void store(std::ostream& out);

		//fonction inverse : recharger les propriétés sur le fichier
		void store(std::ostream& out, const std::string& title);

		//Associer la valeur à la clé donnée dans la map
		void set_property(const std::string key, std::string value) {
			back[key] = value;
		}

		//Verifier si la valeur associé à la clé name se trouve dans la map 
		bool has_property(const std::string& name) const {
			using namespace std;
			map<string, string>::const_iterator i = back.find(name);

			if (i == back.end())
				return false;
			else
				return true;
		}

		//Retrouner la valeur associé à la clé name 
		std::string get_property(const std::string& name) const {
			return back.find(name)->second;
		}

	};


}
#endif /*PROPERTIES_HPP_*/
