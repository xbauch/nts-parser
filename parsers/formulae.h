#include "parsers/tokens.h"
#include "parsers/generals.h"
#include "parsers/terms.h"
//#include "parsers/atomics.h"

#pragma once

build_op< quantifier, qkind::forall > bop_qf;
build_op< quantifier, qkind::exists > bop_qe;

//-------------------------------------------------
//--------------------Quantifier parser------------
//<quantifier> ::= { forall,
//                   exists }
const handle< quantifier > for_quant() {
  return log( "forall", all( bop_qf, forall_tok ) );
}

const handle< quantifier > exi_quant() {
  return log( "exists", all( bop_qe, exists_tok ) );
}

template<>
const handle< quantifier > parser< quantifier > =
  strict( "invalid quantifier form",
          attempt( for_quant() )
       ||          exi_quant() );
//--------------------------------------------------
