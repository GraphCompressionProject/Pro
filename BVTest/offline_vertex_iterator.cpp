#include "stdafx.h"
#include "offline_vertex_iterator.hpp"

#include <sstream>
#include <iterator>
#include <algorithm>

namespace webgraph { namespace ascii_graph {
	

//constructeur : itérateur sur aucun fichier de graphe (vide) : par défaut : marker à true 
offline_vertex_iterator::offline_vertex_iterator() {
   init();
   end_marker = true;
} 


//constructeur : itérateur sur le fichier "filename"



offline_vertex_iterator::offline_vertex_iterator(const char* filename)
{
	
	
	init(); // mettre à zéro les attributs 

	this->filename = filename; // le nom du fichier du graphe courant 
	back.open(filename); // ouvrir le fichier dans le flux back 

	end_marker = false; // marker à false 

	string tmp;
	getline(back, tmp);  //récupérer la première ligne qui contient le nombre de noeuds du graphe 
	istringstream iss(tmp);
	iss >> num_vertices; // MAJ le nombre de noeuds 

	//current_descriptor.label_ref() = 0; // just to make sure it's valid.

	//cout << "offline vertex iterator" << endl;
	increment();

	//cout << "offline vertex iterator after increment" << endl;

	//current_descriptor.label_ref() = 0;
}

//constructeur : copier l'iterateur courant dans l'iterateur that 
offline_vertex_iterator::offline_vertex_iterator( const offline_vertex_iterator& that ) {
   init();
   copy(that);
}


//redéfinition de = par rapport à deux itérateur ( copier le courant dans rhs) 
offline_vertex_iterator& offline_vertex_iterator::operator = (const offline_vertex_iterator& rhs ) {
   copy( rhs );
   return *this;
}


// copier l'itérateur other dans le courant 
void offline_vertex_iterator::copy( const offline_vertex_iterator& other ) {
   if( other.end_marker ) {
      if( back.is_open() )
         back.close();
      
      end_marker = true;  
   } else {
      current_descriptor = other.current_descriptor;
      if( back.is_open())
         back.close();

      back.clear();
      
      back.open( other.filename.c_str() );
      back.seekg( other.get_pos );
      get_pos = other.get_pos;
      num_vertices = other.num_vertices;
      end_marker = false;
      filename = other.filename;
   }
}






void offline_vertex_iterator::increment() {
	string tmp;

	bool success = (bool)getline(back, tmp);
	// si ligne non récupérée
	if (!success) {
		end_marker = true;
		//cout << "not success " << endl;
#if 0
		cerr << "Just failed to perform a getline. Current state: " << endl
			<< as_str() << endl
			<< "back.good() = " << back.good() << endl;
#endif      
		return;
	}
	//sinon 

	//cout << "dans increment" << endl;
	string delimiter = " : ";
	size_t pos = 0;
	int node_label;

	// récupérer l'id du noeud 
	pos = tmp.find(delimiter);
	string token = tmp.substr(0, pos);
	istringstream iss(token);
	iss >> node_label;
	iss.clear();
	//cout << "node id : " << node_label << endl;

	//récupérer la liste des successeurs 
	tmp.erase(0, pos + delimiter.length());
	iss.str(tmp);

	// vider la liste des successseurs du noeud courant
	current_descriptor.successors_ref().clear();


	//copier les successeurs récupéré dans la liste du noeud courant 
	std::copy(istream_iterator<vertex_label_t>(iss), istream_iterator<vertex_label_t>(), back_inserter(current_descriptor.successors_ref()));

	//mettre à jour la position dans le fichier du noeud courant 
	get_pos = back.tellg();

	//mettre à jour le label du noeud courant 
	///current_descriptor.label_ref()++;
	current_descriptor.label_ref()= node_label;

	//cout << "noeud courrant" << current_descriptor.label_ref() << endl;
	//cout << " his succ nb: " << current_descriptor.successors_ref().size() << endl;
}


//toString de l'instance 
string offline_vertex_iterator::as_str() const {
   ostringstream o;
   
   o << "ascii_graph::offline_vertex_iterator\n"
     << "\tcurrent_descriptor:\n\t" << current_descriptor.as_str() << "\n"
     << "\tfilename: " << filename << "\n"
     << "\tseek position: " << get_pos << "\n"
     << "\tis end marker?: " << end_marker << "\n";
   
   return o.str();
}


//destructeur 
offline_vertex_iterator::~offline_vertex_iterator()
{
}


//retourner le outdegree du noeud courant 
int outdegree( const offline_vertex_iterator& me ) {
   return me.current_descriptor.get_successors().size();   
}
      

//retourner les successeurs du noeud courant 
const std::vector<vertex_label_t>& successors( const offline_vertex_iterator& me ) {
   return me.current_descriptor.get_successors();
}



} }
