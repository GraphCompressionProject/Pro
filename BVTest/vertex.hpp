#ifndef ASCIIGRAPHNODE_HPP
#define ASCIIGRAPHNODE_HPP
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>

namespace webgraph { namespace ascii_graph {

typedef unsigned int vertex_label_t;

// un noeud est décrit par son label et ses successeurs 

class vertex_descriptor {

private:
   vertex_label_t label;
   std::vector<vertex_label_t> successors;
   bool __successors_loaded;

public:
  
   //constructeur 
   vertex_descriptor() { __successors_loaded = false; } 
   vertex_descriptor( vertex_label_t l ) {label = l; __successors_loaded = false; }

  //conversion de type 
   operator vertex_label_t() const { return label; }

   //redefinition des opérateurs << et == par rapport à vertex_description 
   //mettre le label du noeud v dans out (concatener) 
   friend std::ostream& operator << (std::ostream& out, const vertex_descriptor& v ); // écrire la description du noeud en flux de donnée (stream) 
   
   //verifier si le noeud rhs est égale au noeud courant (l'instance) 
   bool operator == ( const vertex_descriptor& rhs ) const { return label == rhs.label && successors == rhs.successors; }


   //getter et setter 
   bool successors_loaded() const { return __successors_loaded; };
   void successors_loaded( bool sl ) { __successors_loaded = sl; }


   const vertex_label_t& get_label() const { return label; }
   vertex_label_t& label_ref() { return label; }

   const std::vector<vertex_label_t>& get_successors() const {
      if( __successors_loaded ) {
         return successors;
      } else {
        throw std::logic_error( "Attempt to get successors of descriptor which does not have them loaded." );
      }}
   std::vector<vertex_label_t>& successors_ref() { __successors_loaded = true; return successors; }


   std::string as_str() const;
};

} }

#endif
