#ifndef COMPRESSION_FLAGS_HPP_
#define COMPRESSION_FLAGS_HPP_

namespace webgraph {
	namespace compression_flags {
		const int DELTA = 1;
  		const int GAMMA = 2;
   
		const int UNARY = 7;
  		const int ZETA = 8;
  		const int NIBBLE = 9;
//  
//  public static final int NONE = 255;
	
		extern const char* CODING_NAME[]; // = [ "something", "something_else" ];

	};
};

#endif /*COMPRESSION_FLAGS_HPP_*/
