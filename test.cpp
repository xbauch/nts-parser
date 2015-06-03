#include <iostream>
#include <sstream>

using namespace std;

#include "tools/nts2Nts.h"

nts::Nts* nts_to_Nts( nts_parser::nts nn ) {
  nts_2_Nts Nn( nn );
  return Nn.n;
}

template< typename T >
nts_parser::eref< T > file_to( const char* file ) {
  stream_range in( file ); 
  auto const q_parser = nts_parser::commented_first
                     && nts_parser::parser< T >
                     && nts_parser::is_last;
  nts_parser::eref< T > q {};
  typename stream_range::iterator i = in.first;
  if ( !q_parser( i, in, &q ) )
    std::cerr << "Parser Error.\n";
  return q;
}

nts::Nts* file_to_Nts( const char* file ) {
  return nts_to_Nts( nts_parser::deitemise( file_to< nts_parser::nts >( file ) ) );
}

int main( int argc, char** argv ) {
  auto p = file_to_Nts( argv[ 1 ] );

  std::cout << *p << std::endl;
}
