#include <type_traits>
#include <iostream>
#include <sstream>

#include "tools/data.h"
#include "tools/logic.h"
#include "parsers/formulae.h"

using namespace std;

template< typename T >
eref< T > parse( char* file ) {
  stream_range in( file ); 
  auto const q_parser = first_token && parser< T > && is_last;
  eref< T > q {};
  typename stream_range::iterator i = in.first;
  if ( q_parser( i, in, &q ) )
    cout << "ok" << endl;
  else
    cout << "nok" << endl;
  return q;
}

int main( int argc, char** argv ) {
  if ( argc <= 1 ) {
    cerr << "no input files" << endl;
    return 1;
  }

  cout << deitemise( parse< formula >( argv[ 1 ] ) ).print() << "foo" << endl;
  return 0;
}
