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

		//d�structeur 
		virtual ~properties();

		//Charger les propri�t�s du fichier dans la map "back"
		void load(std::istream& in);

		void store(std::ostream& out);

		//fonction inverse : recharger les propri�t�s sur le fichier
		void store(std::ostream& out, const std::string& title);

		//Associer la valeur � la cl� donn�e dans la map
		void set_property(const std::string key, std::string value) {
			back[key] = value;
		}

		//Verifier si la valeur associ� � la cl� name se trouve dans la map 
		bool has_property(const std::string& name) const {
			using namespace std;
			map<string, string>::const_iterator i = back.find(name);

			if (i == back.end())
				return false;
			else
				return true;
		}

		//Retrouner la valeur associ� � la cl� name 
		std::string get_property(const std::string& name) const {
			return back.find(name)->second;
		}

	};


}
#endif /*PROPERTIES_HPP_*/
