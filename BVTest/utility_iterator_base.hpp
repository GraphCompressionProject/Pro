#ifndef UTILITY_ITERATOR_BASE_HPP
#define UTILITY_ITERATOR_BASE_HPP

#include <utility>
#include <string>

namespace webgraph { namespace bv_graph { namespace utility_iterators {
   
	// classe abstraite d'itérateur 
   template<typename val_type> 
   class utility_iterator_base {
   public:
	   
	   //Constructeur et destructeur par défaut 
      utility_iterator_base() {}
      virtual ~utility_iterator_base() {}
	  
	  // retourner le prochain élément 
      virtual val_type next() = 0;

	  // vrai si il y'a un prochain élément 
      virtual bool has_next() const = 0;

	  //passer un certain nombre d'éléments 
      virtual int skip( int how_many ) = 0;
	  
	  //to string 
      virtual std::string as_str() const = 0;

      // utiliser cette méthode pour le polymorphisme en c++ 
      virtual utility_iterator_base<val_type>* clone() const = 0;
   };
} } }

#endif
