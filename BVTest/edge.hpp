#ifndef ASCIIGRAPHEDGE_HPP
#define ASCIIGRAPHEDGE_HPP

#include "vertex.hpp"
#include <utility>
#include <iostream>

namespace webgraph { namespace ascii_graph {

   typedef std::pair<vertex_descriptor, vertex_descriptor> edge;

   //redfinition de l'operateur << entre un edge et ostream 
   //mettre dans out : "(noeud1.label1, noeud2.label2) 
   std::ostream& operator << ( std::ostream& out, const edge& e );   
} }

#endif
