#ifndef EMPTY_ITERATOR_HPP
#define EMPTY_ITERATOR_HPP

#include "utility_iterator_base.hpp"
#include <string>
#include <exception>
#include <stdexcept>

namespace webgraph { namespace bv_graph { namespace utility_iterators {

// itérateur vide 
class empty_iterator : public utility_iterator_base<int> {
public:
   empty_iterator() {}

   int next() {
      throw std::logic_error( "Can't increment empty iterator." );
   }

   bool has_next() const {
      return false;
   }
   
   std::string as_str() const {
      return "Empty iterator.";
   }

   int skip( int how_many ) {
      throw std::logic_error( "Can't skip on an empty iterator." );
   }
   
   empty_iterator* clone() const {
      return new empty_iterator();
   }
};

}}}

#endif
