#include "stdafx.h"
#include "edge.hpp"

namespace webgraph { namespace ascii_graph { 

std::ostream& operator << ( std::ostream& out, const edge& e ) {
   out << "(" << e.first.get_label() << ", " << e.second.get_label() << ")";
   return out;
}

} }

