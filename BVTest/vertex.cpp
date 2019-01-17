#include "stdafx.h"
#include "vertex.hpp"
#include <iostream>
#include <iterator>
#include <algorithm>
#include <sstream>

namespace webgraph { namespace ascii_graph {

std::ostream& operator << (std::ostream& out, const vertex_descriptor& v ) {
   using namespace std;
   out << v.label;
//    out << v.label << " : ";
//    copy( v.successors.begin(), v.successors.end(), ostream_iterator<vertex_label_t>( out, " " ) );
//    out << "\n";
 return out;}

// afficher le label et les voisins d'un noeud 
std::string vertex_descriptor::as_str() const {
   using namespace std;
   ostringstream o;
   o << label << " : ";
   copy( successors.begin(), successors.end(), ostream_iterator<vertex_label_t>( o, " " ) );
   return o.str();
}


} }
