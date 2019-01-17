#ifndef UTILITY_ITERATOR_BASE_HPP
#define UTILITY_ITERATOR_BASE_HPP

#include <utility>
#include <string>

namespace webgraph { namespace bv_graph { namespace utility_iterators {
   
	// classe abstraite d'it�rateur 
   template<typename val_type> 
   class utility_iterator_base {
   public:
	   
	   //Constructeur et destructeur par d�faut 
      utility_iterator_base() {}
      virtual ~utility_iterator_base() {}
	  
	  // retourner le prochain �l�ment 
      virtual val_type next() = 0;

	  // vrai si il y'a un prochain �l�ment 
      virtual bool has_next() const = 0;

	  //passer un certain nombre d'�l�ments 
      virtual int skip( int how_many ) = 0;
	  
	  //to string 
      virtual std::string as_str() const = 0;

      // utiliser cette m�thode pour le polymorphisme en c++ 
      virtual utility_iterator_base<val_type>* clone() const = 0;
   };
} } }

#endif
