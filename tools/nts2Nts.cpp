// #include <iostream>
// #include <sstream>

// #include "tools/nts2Nts.h"

using namespace std::placeholders;
using nts_parser::deitemise;
using nts_parser::itemise;
using nts_parser::idn;
using nts_parser::map_replace;
using AT = nts_parser::arith_term;
using VV = nts_2_Nts::VV;
using VR = nts_2_Nts::VR;
using VS = nts_2_Nts::VS;
using tsTV = nts_2_Nts::tsTV;
using pTV = nts_2_Nts::pTV;
using pTT = nts_2_Nts::pTT;
using VT = nts_2_Nts::VT;
using Vs = nts_2_Nts::Vs;
using uF = nts_2_Nts::uF;
using uT = nts_2_Nts::uT;

nts_2_Nts::nts_2_Nts( nts_parser::nts in ) {
  process_nts_name( in.get< nts_parser::nts_name >( 0 ) );
  process_decl_glob( in.get< nts_parser::decl_glob >( 1 ) );
  process_init( in.get< nts_parser::init >( 2 ) );
  process_instances( in.get< nts_parser::instances >( 3 ) );
  process_nts_list( in.get< nts_parser::nts_list >( 4 ) );
}

//------------------------------------//
// Declarations                       //
//------------------------------------//

void nts_2_Nts::process_nts_name( nts_parser::nts_name nn ) {
  n = new nts::Nts( nn.get< idn >( 1 ).print() );
  process_annotations( nn.get< nts_parser::annotations >( 0 ),
                       n->annotations );
}

std::vector< nts::AnnotString* > nts_2_Nts::a2a( nts_parser::annotations as ) {
  return map_replace< vector< nts::AnnotString* > >(
    [&] (auto r) {
      nts_parser::annotation a = deitemise< nts_parser::annotation >( r );
      nts_parser::a_type_val atv = a.get< nts_parser::a_type_val >( 1 );
      assert( atv.flat[ 0 ] == nts_parser::a_type_val::kind::astring );
      return new nts::AnnotString(
        a.get< idn >( 0 ).print(),
        atv.get< nts_parser::astring >( 1 ).name
      );
    },
    as.flat
  );
}

void nts_2_Nts::process_annotations( nts_parser::annotations a,
                                     nts::Annotations& A ) {
  auto as = a2a( a );
  for ( auto an : as )
    an->insert_to( A );
}
    
void nts_2_Nts::process_decl_glob( nts_parser::decl_glob dg ) {
  VV vv;
  for ( auto r : dg.flat ) {
    VV v = variables_from_decl( deitemise< nts_parser::decl >( r ), glob_vars );
    std::move( v.begin(), v.end(), back_inserter( vv ) );
  }
  for ( auto v : vv )
    v->insert_to( *n );
}

VV nts_2_Nts::variables_from_decl( nts_parser::decl d, NamMap& scope ) {
  if ( d.is_par() )
    return variables_from_decl_par_blocks(
      d.get< nts_parser::decl_par_blocks >( 1 ), scope
    );
  else
    return variables_from_decl_blocks(
      d.get< nts_parser::decl_blocks >( 1 ), scope
    );
}

VV nts_2_Nts::variables_from_decl_lits( nts_parser::decl_lits dl,
                                        nts::ScalarType st, NamMap& scope ) {
  return map_replace< VV >(
    [&] (auto r) {
      return this->variable_from_decl_lit( deitemise< nts_parser::decl_lit >( r ),
                                           st, scope );
    },
    dl.flat
  );
}

nts::Variable* nts_2_Nts::variable_from_decl_lit( nts_parser::decl_lit dl,
                                                  nts::ScalarType st,
                                                  NamMap& scope ) {
  return register_variable( name_from_decl_lit( dl ),
                            datatype_from_decl_lit( dl, scope, st ),
                            scope );
}

void nts_2_Nts::process_init( nts_parser::init i ) {
  n->initial_add_conjunct( formula_from_init( i ) );
}

void nts_2_Nts::process_instances( nts_parser::instances i ) {
  for ( auto r : i.get< nts_parser::inst_list >( 1 ).flat )
    process_instance( deitemise< nts_parser::instance >( r ) );
}

void nts_2_Nts::process_instance( nts_parser::instance i ) {
  std::string name = i.get< idn >( 0 ).print();
  nts::BasicNts* bn = new nts::BasicNts( name );
  bntses[ name ] = bn;
  uT inst_term = term_from_arith_term( i.get< AT >( 1 ),
                                       glob_vars );
  nts::Instance* in = new nts::Instance( bn, inst_term.release() );
  in->insert_to( *n );
}

void nts_2_Nts::process_nts_list( nts_parser::nts_list nl ) {
  for ( auto r : nl.flat )
    process_nts_basic( deitemise< nts_parser::nts_basic >( r ) );
}

void nts_2_Nts::process_nts_basic( nts_parser::nts_basic nb ) {
  nts::BasicNts* bn = bntses[ nb.get< idn >( 1 ).print() ];
  process_annotations( nb.get< nts_parser::annotations >( 0 ), bn->annotations );
  process_nts_body( nb.get< nts_parser::nts_body >( 2 ), *bn );
  bn->insert_to( *n );
}

void nts_2_Nts::process_nts_body( nts_parser::nts_body nb, nts::BasicNts& bn ) {
  NamMap loc_vars;
  process_decl_loc( nb.get< nts_parser::decl_loc >( 0 ), bn, loc_vars );
  StateMap loc_states;
  process_statemarks( nb.get< nts_parser::statemarks >( 1 ), bn, loc_states );
  
  VR vt = transitions_from_transitions( nb.get< nts_parser::transitions >( 2 ),
                                        loc_vars,
                                        loc_states,
                                        bn );
  for ( auto t : vt )
    t->insert_to( bn );
}

void nts_2_Nts::process_decl_loc( nts_parser::decl_loc dl, nts::BasicNts& bn,
                                  NamMap& scope ) {
  VV vv = variables_from_io( dl.get< nts_parser::in >( 0 ), scope );
  for ( auto v : vv )
    v->insert_param_in_to( bn );
            
  vv = variables_from_io( dl.get< nts_parser::out >( 1 ), scope );
  for ( auto v : vv )
    v->insert_param_out_to( bn );

  vv.clear();
  for ( auto r : dl.flat ) {
    VV v = variables_from_decl( deitemise< nts_parser::decl >( r ), scope );
    std::move( v.begin(), v.end(), std::back_inserter( vv ) );
  }
  
  for ( auto v : vv )
    v->insert_to( bn );
}

void nts_2_Nts::process_statemarks( nts_parser::statemarks s, nts::BasicNts& bn,
                                    StateMap& sm ) {
  reg_states( states_from_states( s.get< nts_parser::states >( 0 ) ),
              [&] (auto s) {}, bn, sm );
  nts_parser::states_init si = s.get< nts_parser::states_init >( 1 );
  assert( !si.flat.empty() );
  reg_states( states_from( si ),
              [&] (auto s) { s->is_initial() = true; }, bn, sm );
  reg_states( states_from( s.get< nts_parser::states_fin >( 2 ) ),
              [&] (auto s) { s->is_final() = true; }, bn, sm );
  reg_states( states_from( s.get< nts_parser::states_err >( 3 ) ),
              [&] (auto s) { s->is_error() = true; }, bn, sm );
}

VR nts_2_Nts::transitions_from_transitions( nts_parser::transitions t,
                                            NamMap& scope, StateMap& sm,
                                            nts::BasicNts& bn ) {
  return map_replace< VR >(
    [&] (auto r) {
      return this->transition_from_transition( deitemise< nts_parser::transition >( r ),
                                               scope, sm, bn );
    },
    t.flat
  );
}

VS nts_2_Nts::states_from_statelist_a( nts_parser::statelist_a s ) {
  nts::State* st = new nts::State( s.get< idn >( s.flat.size() - 1 ).print() );
  process_annotations( s.get< nts_parser::annotations >( s.flat.size() - 2 ),
                       st->annotations );
  if ( s.flat.size() > 2 )
    return states_from_statelist( s.get< nts_parser::statelist >( 0 ) );
  return VS();
}

VS nts_2_Nts::states_from_statelist( nts_parser::statelist s ) {
  return map_replace< VS >( [&] (auto r) {
      return new nts::State( deitemise< idn >( r ).print() );
    }, s.flat );
}

nts::Transition* nts_2_Nts::transition_from_transition( nts_parser::transition t,
                                                        NamMap& scope,
                                                        StateMap& sm,
                                                        nts::BasicNts& bn ) {
  auto fit = t.flat.begin(); //TODO: labels
  if ( t.flat.size() > 4 )
    ++fit;
  int fst = 0;
  if ( t.flat.size() > 4 )
    ++fst;

  VS vs;
  std::string from = t.get< idn >( fst ).print();
  if ( sm.count( from ) == 0 )
    vs.push_back( new nts::State( from ) );
  std::string to = t.get< idn >( fst ).print();
  if ( sm.count( to ) == 0 )
    vs.push_back( new nts::State( to ) );
  reg_states( vs, [&] (auto s) {}, bn, sm );

  nts::Transition* tt = new nts::Transition(
    transition_rule_from_rule( t.get< nts_parser::rule >( fst + 3 ), scope ),
    *sm[ from ],
    *sm[ to ]
  );
  process_annotations( t.get< nts_parser::annotations >( fst + 2 ),
                       tt->annotations );
  return tt;
}

std::unique_ptr< nts::TransitionRule >
nts_2_Nts::transition_rule_from_rule( nts_parser::rule r, NamMap& scope ) {
  using K = nts_parser::rule::kind;
  
  switch ( r.flat[ 0 ] ) {
  case K::r_empty : {
    return std::unique_ptr< nts::TransitionRule >(
      new nts::FormulaTransitionRule(
        uF( new nts::BooleanTerm( term_from_bool( true ) ) )
      )
    );
  }
  case K::r_formula :
    return std::unique_ptr< nts::TransitionRule >(
      new nts::FormulaTransitionRule(
        formula_from_formula( r.get< nts_parser::formula >( 1 ), scope )
      )
    );
  case K::r_call : { //TODO: ignoring havoc
    tsTV t = triple_from_call( r.get< nts_parser::call >( 1 ), scope );
    return std::unique_ptr< nts::TransitionRule >(
      new nts::CallTransitionRule(
        *bntses[ get< 0 >( t ) ],
        get< 1 >( t ),
        get< 2 >( t )
      )
    );
  }
  default : assert( false ); return std::unique_ptr< nts::TransitionRule >();
  }
}

tsTV nts_2_Nts::triple_from_call( nts_parser::call c, NamMap& scope ) {
  return triple_from_call_base( c.get< nts_parser::call_base >( 0 ), scope );
}

tsTV nts_2_Nts::triple_from_call_base( nts_parser::call_base c, NamMap& scope ) {
  using K = nts_parser::call_base::kind;

  int index = (c.flat[ 0 ] == K::no_assign ? 2 : 3);
  pTV p = pair_from_arglist( c.get< nts_parser::arglist >( index ), scope );
  
  switch ( c.flat[ 0 ] ) {
  case K::no_assign : return tsTV(
    c.get< idn >( 1 ).print(),
    p.first,
    p.second
  );
  case K::single_assign : return tsTV(
    c.get< idn >( 2 ).print(),
    p.first,
    { scope[ name_from_idp( c.get< nts_parser::idp >( 1 ) ) ] }
  );
  case K::multi_assign : return tsTV(
    c.get< idn >( 2 ).print(),
    p.first,
    map_replace< VV >( [&] (auto r) {
        return scope[ this->name_from_idp( deitemise< nts_parser::idp >( r ) ) ];
      },
      c.get< nts_parser::ret_terms >( 1 ).flat
    )
  );
  default : assert( false ); return tsTV( "", {}, {} );
  }
}

pTV nts_2_Nts::pair_from_arglist( nts_parser::arglist a, NamMap& scope ) {
  if ( a.flat.empty() )
    return pTV( {}, {} );
  return pair_from_arith_list( a.get< nts_parser::arith_list >( 0 ), scope );
}

pTV nts_2_Nts::pair_from_arith_list( nts_parser::arith_list al, NamMap& scope ) {
  int i = al.flat.size() - 1;
  for ( ; i >= 0; --i )
    if ( !deitemise< AT >( al.flat[ i ] ).is_idp() )
      break;

  ++i;
  return pTV(
    map_replace< VT >(
      [&] (auto r) {
        return this->term_from_arith_term( deitemise< AT >( r ), scope ).release();
      },
      al.flat, al.flat.begin(), al.flat.begin() + i
    ),
    map_replace< VV >(
      [&] (auto r) {
        return scope[ this->name_from_arith_term( deitemise< AT >( r ) ) ];
      },
      al.flat, al.flat.begin() + i, al.flat.end()
    )
  );
}

std::string nts_2_Nts::name_from_arith_term( AT at ) {
  using K = AT::kind;
  using KK = nts_parser::arith_lit;
  
  assert( at.flat[ 0 ] == K::literal );
  KK al = at.get< KK >( 1 );
  assert( al.flat[ 0 ] == KK::arith_id );
  return name_from_id( al.get< nts_parser::id >( 1 ) );
}

VV nts_2_Nts::variables_from_decl_blocks( nts_parser::decl_blocks db,
                                          NamMap& scope ) {
  VV vv;
  for ( auto r : db.flat ) {
    VV v = variables_from_decl_block( deitemise< nts_parser::decl_block >( r ),
                                      scope );
    std::move( v.begin(), v.end(), back_inserter( vv ) );
  }
  return vv;
}

VV nts_2_Nts::variables_from_decl_io_blocks( nts_parser::decl_io_blocks db,
                                             NamMap& scope ) {
  VV vv;
  for ( auto r : db.flat ) {
    VV v = variables_from_decl_io_block( deitemise< nts_parser::decl_io_block >( r ),
                                         scope );
    std::move( v.begin(), v.end(), back_inserter( vv ) );
  }
  return vv;
}

VV nts_2_Nts::variables_from_decl_par_blocks( nts_parser::decl_par_blocks db,
                                              NamMap& scope ) {
  VV vv;
  for ( auto r : db.flat ) {
    VV v = variables_from_decl_par_block( deitemise< nts_parser::decl_par_block >( r ),
                                          scope );
    std::move( v.begin(), v.end(), back_inserter( vv ) );
  }
  return vv;
}

VV nts_2_Nts::variables_from_decl_io_block( nts_parser::decl_io_block db,
                                            NamMap& scope ) {
  nts::ScalarType st = scalartype_from_type( db.get< nts_parser::type >( 1 ) );
  return variables_from_decl_io_lits( db.get< nts_parser::decl_io_lits >( 0 ), st,
                                      scope );
}

VV nts_2_Nts::variables_from_decl_block( nts_parser::decl_block db,
                                         NamMap& scope ) {
  nts::ScalarType st = scalartype_from_type( db.get< nts_parser::type >( 1 ) );
  return variables_from_decl_lits( db.get< nts_parser::decl_lits >( 0 ), st,
                                   scope );
}

VV nts_2_Nts::variables_from_decl_par_block( nts_parser::decl_par_block db,
                                             NamMap& scope ) {
  nts::ScalarType st = scalartype_from_type( db.get< nts_parser::type >( 1 ) );
  return variables_from_decl_par_lits( db.get< nts_parser::decl_par_lits >( 0 ), st,
                                       scope );
}

VV nts_2_Nts::variables_from_decl_io_lits( nts_parser::decl_io_lits dl,
                                           nts::ScalarType st, NamMap& scope ) {
  return map_replace< VV >(
    [&] (auto r) {
      return this->variable_from_decl_io_lit(
        deitemise< nts_parser::decl_io_lit >( r ), st, scope
      );
    },
    dl.flat
  );
}

VV nts_2_Nts::variables_from_decl_par_lits( nts_parser::decl_par_lits dl,
                                            nts::ScalarType st, NamMap& scope ) {
  return map_replace< VV >(
    [&] (auto r) {
      return this->variable_from_decl_par_lit(
        deitemise< nts_parser::decl_par_lit >( r ), st, scope
      );
    },
    dl.flat
  );
}

nts::Variable* nts_2_Nts::variable_from_decl_io_lit( nts_parser::decl_io_lit dl,
                                                     nts::ScalarType st,
                                                     NamMap& scope ) {
  return register_variable( name_from_decl_io_lit( dl ),
                            datatype_from_decl_io_lit( dl, st ),
                            scope );
}

nts::Variable* nts_2_Nts::variable_from_decl_par_lit( nts_parser::decl_par_lit dl,
                                                      nts::ScalarType st,
                                                      NamMap& scope ) {
  return register_variable( name_from_decl_par_lit( dl ),
                            datatype_from_decl_par_lit( dl, scope, st ),
                            scope );
}

//------------------------------------//
// Formulae                           //
//------------------------------------//

uF nts_2_Nts::formula_from_init( nts_parser::init i ) {
  return formula_from_formula( i.get< nts_parser::formula >( 0 ), glob_vars );
}

//TODO
uF nts_2_Nts::formula_from_formula( nts_parser::formula f, NamMap& scope ) {
  using K = nts_parser::formula::kind;
  
  switch( f.flat[ 0 ] ) {
  case K::atomic : return formula_from_atom( f.get< nts_parser::atom >( 1 ),
                                             scope );
  case K::parenthesised :
    return formula_from_formula( f.get< nts_parser::formula >( 1 ), scope );
  case K::boolean :
    return uF(
      new nts::FormulaBop(
        boolop_from_bop( f.get< nts_parser::bop >( 2 ) ),
        formula_from_formula( f.get< nts_parser::formula >( 1 ), scope ),
        formula_from_formula( f.get< nts_parser::formula >( 3 ), scope )
      )
    );
  case K::negated :
    return uF(
      new nts::FormulaNot(
        formula_from_formula( f.get< nts_parser::formula >( 1 ), scope )
      )
    );
  case K::quantified : {
    NamMap q_scope;
    auto q = quantifier_from_quantifier( f.get< nts_parser::quantifier >( 1 ) );
    auto qt = quantifiedtype_from_q_type( f.get< nts_parser::q_type >( 3 ),
                                          scope );
    nts::QuantifiedVariableList qvl( q, qt );
    
    Vs vs = names_from_idn_list( f.get< nts_parser::idn_list >( 2 ) );
    VV vv = register_variables( vs, qt.type(), q_scope );
    
    q_scope.insert( scope.begin(), scope.end() );
    nts::QuantifiedFormula* qf(
      new nts::QuantifiedFormula(
        q,
        qt,
        formula_from_formula( f.get< nts_parser::formula >( 4 ), q_scope )
      )
    );
    for ( auto v : vv )
      v->insert_to( qf->list );

    return uF( qf );
  }
  default : assert( false ); return nullptr;
  }
}

uF nts_2_Nts::formula_from_atom( nts_parser::atom a, NamMap& scope ) {
  using K = nts_parser::atom::kind;
  
  switch ( a.flat[ 0 ] ) {
  case K::boolean_term :
    return formula_from_bool_term( a.get< nts_parser::bool_term >( 1 ), scope );
  case K::relation :
    return uF(
      new nts::Relation(
        relationop_from_rop( a.get< nts_parser::rop >( 2 ) ),
        term_from_arith_term( a.get< AT >( 1 ), scope ),
        term_from_arith_term( a.get< AT >( 3 ), scope )
      )
    );
  case K::assign :
    return formula_from_assignment( a.get< nts_parser::assignment >( 1 ), scope );
  case K::havoced :
    return formula_from_havoc( a.get< nts_parser::havoc >( 1 ), scope );
  default : assert( false ); return nullptr;
  }
}

nts::Quantifier nts_2_Nts::quantifier_from_quantifier( nts_parser::quantifier q ) {
  if ( q.flat[ 0 ] == nts_parser::quantifier::kind::forall )
    return nts::Quantifier::Forall;
  else
    return nts::Quantifier::Exists;
}

nts::QuantifiedType nts_2_Nts::quantifiedtype_from_q_type( nts_parser::q_type qt,
                                                           NamMap& scope ) {
  nts::DataType dt( scalartype_from_type( qt.get< nts_parser::type >( 1 ) ) );
  if ( qt.flat[ 0 ] == nts_parser::q_type::kind::unbound )
    return nts::QuantifiedType( dt );
  else
    return nts::QuantifiedType(
      dt,
      term_from_arith_term( qt.get< AT >( 2 ), scope ),
      term_from_arith_term( qt.get< AT >( 3 ), scope )
    );
}

Vs nts_2_Nts::names_from_idn_list( nts_parser::idn_list il ) {
  return map_replace< Vs >( [&] (auto r) {
      return deitemise< idn >( r ).print();
    }, il.flat );
}

VV nts_2_Nts::register_variables( Vs names, nts::DataType type, NamMap& scope ) {
  return map_replace< VV >( [&] (auto s) {
      return this->register_variable( s, type, scope );
    }, names );
}

nts::Variable* nts_2_Nts::register_variable( std::string name, nts::DataType dt,
                                             NamMap& scope ) {
  nts::Variable* v = new nts::Variable( dt, name );
  scope[ name ] = v;
  return v;
}

nts::Variable* nts_2_Nts::lookup_variable( std::string name, NamMap& l_scope ) {
  if ( l_scope.count( name ) > 0 )
    return l_scope[ name ];
  assert( glob_vars.count( name ) > 0 );
  return glob_vars[ name ];
}

uF nts_2_Nts::formula_from_bool_term( nts_parser::bool_term bt, NamMap& scope ) {
  using K = nts_parser::bool_term::kind;
  
  switch( bt.flat[ 0 ] ) {
  case K::literal :
    return formula_from_bool_lit( bt.get< nts_parser::bool_lit >( 1 ), scope );
  case K::parenthesised :
    return formula_from_bool_term( bt.get< nts_parser::bool_term >( 1 ), scope );
  case K::negated :
    return uF(
      new nts::FormulaNot(
        formula_from_bool_term( bt.get< nts_parser::bool_term >( 1 ), scope )
      )
    );
  case K::relation :
    return uF(
      new nts::FormulaBop(
        boolop_from_bop( bt.get< nts_parser::bop >( 2 ) ),
        formula_from_bool_term( bt.get< nts_parser::bool_term >( 1 ), scope ),
        formula_from_bool_term( bt.get< nts_parser::bool_term >( 3 ), scope )
      )
    );
  default : assert( false ); return nullptr;
  }
}

nts::BoolOp nts_2_Nts::boolop_from_bop( nts_parser::bop o ) {
  using K = nts_parser::bop::kind;
  using B = nts::BoolOp;
  
  switch( o.flat[ 0 ] ) {
  case K::band : return B::And;
  case K::bor : return B::Or;
  case K::imply : return B::Imply;
  case K::equiv : return B::Equiv;
  default : assert( false ); return B::And;
  }
}

nts::RelationOp nts_2_Nts::relationop_from_rop( nts_parser::rop r ) {
  using K = nts_parser::rop::kind;
  using R = nts::RelationOp;
  
  switch( r.flat[ 0 ] ) {
  case K::rop_equ : return R::eq;
  case K::rop_neq : return R::neq;
  case K::rop_leq : return R::leq;
  case K::rop_ltn : return R::lt;
  case K::rop_geq : return R::geq;
  case K::rop_gtn : return R::gt;
  default : assert( false ); return R::eq;
  }
}

nts::ArithOp nts_2_Nts::arithop_from_aop( nts_parser::aop o ) {
  using K = nts_parser::aop::kind;
  using B = nts::ArithOp;
  
  switch( o.flat[ 0 ] ) {
  case K::plus : return B::Add;
  case K::minus : return B::Sub;
  case K::times : return B::Mul;
  case K::div : return B::Div;
  case K::srem : return B::Mod;
  default : assert( false ); return B::Add;
  }
}

uF nts_2_Nts::formula_from_assignment( nts_parser::assignment a,
                                                  NamMap& scope ) {
  using K = nts_parser::assignment::kind;

  if ( a.flat[ 0 ] == K::as_idp )
    return uF(
      new nts::Relation(
        relationop_from_rop( a.get< nts_parser::rop >( 2 ) ),
        term_from_idp( a.get< nts_parser::idp >( 1 ), scope ),
        term_from_arith_term( a.get< AT >( 3 ), scope )
      )
    );

  assert( a.get< nts_parser::rop >( 3 ).flat[ 0 ] == nts_parser::rop::kind::rop_equ );
  auto p = pterms_from_multi( a.get< nts_parser::multi >( 2 ), scope );
  VT vals;
  if (a.flat[ 0 ] == K::as_arr_arr) {
    vals = terms_from_arith_list( a.get< nts_parser::arith_list >( 4 ), scope );
  }
  else
    vals = { term_from_arith_term( a.get< AT >( 4 ), scope ).release() };

  return uF(
    new nts::ArrayWrite(
      *scope[ a.get< nts_parser::idp >( 1 ).print() ],
      p.first,
      p.second,
      vals
    )
  );
}

uF nts_2_Nts::formula_from_havoc( nts_parser::havoc h, NamMap& scope ) {
  nts::Havoc* hh = new nts::Havoc();
  
  for ( auto r : h.flat )
    hh->variables.push_back( lookup_variable( deitemise< idn >( r ).print(),
                                              scope ) );
  return uF( hh );
}

uF nts_2_Nts::formula_from_bool_lit( nts_parser::bool_lit bl, NamMap& scope ) {
  using K = nts_parser::bool_lit::kind;
  
  switch( bl.flat[ 0 ] ) {
  case K::bl_id :
    return uF(
      new nts::BooleanTerm(
        term_from_id( bl.get< nts_parser::id >( 1 ), scope )
      )
    );
  case K::bl_array :
    return uF(
      new nts::BooleanTerm(
        term_from_array_read( bl.get< nts_parser::array_read >( 1 ), scope )
      )
    );
  case K::bl_true :
    return uF(
      new nts::BooleanTerm(
        term_from_bool( true )
      )
    );
  case K::bl_false :
    return uF(
      new nts::BooleanTerm(
        term_from_bool( false )
      )
    );
  default : assert( false ); return nullptr;
  }
}

uT nts_2_Nts::term_from_idp( nts_parser::idp i, NamMap& scope ) {
  return uT(
    new nts::VariableReference(
      *lookup_variable( i.print(), scope ),
      true
    )
  );
}

uT nts_2_Nts::term_from_id( nts_parser::id i, NamMap& scope ) {
  return uT(
    new nts::VariableReference(
      *lookup_variable( name_from_id( i ), scope ),
      i.flat[ 0 ] == nts_parser::id::kind::id_p
    )
  );
}

std::string nts_2_Nts::name_from_id( nts_parser::id i ) {
  std::string name = i.print();
  if ( i.flat[ 0 ] == nts_parser::id::kind::id_p )
    name.pop_back();
  return name;
}

std::string nts_2_Nts::name_from_idp( nts_parser::idp i ) {
  std::string name = i.print();
  name.pop_back();
  return name;
}

uT nts_2_Nts::term_from_bool( bool b ) {
  return uT(
    new nts::UserConstant(
      nts::DataType( nts::ScalarType::BitVector( 1 ) ),
      (b ? "true" : "false")
    )
  );
}

pTT nts_2_Nts::pterms_from_multi( nts_parser::multi m, NamMap& scope ) {
  if ( m.flat.empty() )
    return pTT( VT(), VT() );
  return pTT(
    map_replace< VT >(
      [&] (auto r) {
        auto uptr = this->term_from_arith_term( deitemise< AT >( r ), scope );
        return uptr.release();
      },
      m.flat, m.flat.begin() + 1, m.flat.begin() + m.flat[ 0 ] + 1
    ),
    map_replace< VT >(
      [&] (auto r) {
        auto uptr = this->term_from_arith_term( deitemise< AT >( r ), scope );
        return uptr.release();
      },
      m.flat, m.flat.begin() + m.flat[ 0 ] + 1, m.flat.end()
    )
  );
}

VT nts_2_Nts::terms_from_arith_list( nts_parser::arith_list al, NamMap& scope ) {
  return map_replace< VT >(
    [&] (auto r) {
      auto uptr = this->term_from_arith_term( deitemise< AT >( r ), scope );
      return uptr.release();
    },
    al.flat
  );
}

//------------------------------------//
// Types                              //
//------------------------------------//

nts::ScalarType nts_2_Nts::scalartype_from_type( nts_parser::type t ) {
  using K = nts_parser::type::kind;
  
  switch ( t.flat[ 0 ] ) {
  case K::integer :   return nts::ScalarType::Integer();
  case K::real :      return nts::ScalarType::Real();
  case K::boolean :   return nts::ScalarType::BitVector( 1 );
  case K::bitvector : return nts::ScalarType::BitVector( t.flat[ 1 ] );
  default : assert( false );   return nts::ScalarType();
  }
}

nts::DataType nts_2_Nts::datatype_from_decl_lit( nts_parser::decl_lit dl,
                                                 NamMap& scope,
                                                 nts::ScalarType st ) {
  using K = nts_parser::decl_lit::kind;
  
  switch ( dl.flat[ 0 ] ) {
  case K::dl_basic : return nts::DataType( st );
  case K::dl_array : {
    VT tempV = arr_size_from_array( dl.get< nts_parser::nts_array >( 1 ), scope ); 
    return nts::DataType( st,
                          0,
                          tempV );
  }
  case K::dl_aref : {
    VT tempV = arr_size_from_array_ref( dl.get< nts_parser::array_ref >( 1 ),
                                        scope );
    return nts::DataType( st,
                          dl.get< nts_parser::array_ref >( 1 ).flat[ 2 ],
                          tempV );
  }
  default : assert( false ); return nts::DataType();
  }
}

nts::DataType nts_2_Nts::datatype_from_decl_io_lit( nts_parser::decl_io_lit dl,
                                                    nts::ScalarType st ) {
  if ( dl.flat[ 0 ] == nts_parser::decl_io_lit::dil_basic )
    return nts::DataType( st );
  else
    return nts::DataType( st,
                          dl.get< nts_parser::array_pure_ref >( 1 ).flat[ 1 ] );
}

nts::DataType nts_2_Nts::datatype_from_decl_par_lit( nts_parser::decl_par_lit dl,
                                                     NamMap& scope,
                                                     nts::ScalarType st ) {
  if ( dl.flat[ 0 ] == nts_parser::decl_par_lit::dpl_basic )
    return nts::DataType( st );
  else {
    VT tempV = arr_size_from_array( dl.get< nts_parser::nts_array >( 1 ), scope );
    return  nts::DataType( st,
                           0,
                           tempV );
  }
}

//------------------------------------//
// Names                              //
//------------------------------------//

std::string nts_2_Nts::name_from_decl_lit( nts_parser::decl_lit dl ) {
  using K = nts_parser::decl_lit::kind;
  
  switch ( dl.flat[ 0 ] ) {
  case K::dl_basic :
    return name_from_basic( dl.get< nts_parser::basic >( 1 ) );
  case K::dl_array :
    return name_from_basic(
      dl.get< nts_parser::nts_array >( 1 ).get< nts_parser::basic >( 0 )
    );
  case K::dl_aref :
    return name_from_array_ref( dl.get< nts_parser::array_ref >( 1 ) );
  default : assert( false ); return "";
  }
}

std::string nts_2_Nts::name_from_decl_io_lit( nts_parser::decl_io_lit dl ) {
  if ( dl.flat[ 0 ] == nts_parser::decl_io_lit::kind::dil_basic )
    return name_from_basic( dl.get< nts_parser::basic >( 1 ) );
  else
    return name_from_array_pure_ref( dl.get< nts_parser::array_pure_ref >( 1 ) );
}

std::string nts_2_Nts::name_from_decl_par_lit( nts_parser::decl_par_lit dl ) {
  if ( dl.flat[ 0 ] == nts_parser::decl_par_lit::kind::dpl_basic )
    return name_from_basic( dl.get< nts_parser::basic >( 1 ) );
  else
    return name_from_array_ref( dl.get< nts_parser::array_ref >( 1 ) );
}

std::string nts_2_Nts::name_from_array_ref( nts_parser::array_ref ar ) {
  using K = nts_parser::array_ref::kind;
  
  switch ( ar.flat[ 0 ] ) {
  case K::ref_basic :
    return name_from_basic( ar.get< nts_parser::basic > ( 1 ) );
  case K::ref_array :
    return name_from_basic(
      ar.get< nts_parser::nts_array >( 1 ).get< nts_parser::basic >( 0 )
    );
  default : assert( false ); return "";
  }
}

std::string nts_2_Nts::name_from_array_pure_ref( nts_parser::array_pure_ref ar ) {
  return name_from_basic( ar.get< nts_parser::basic >( 0 ) );
}

std::string nts_2_Nts::name_from_basic( nts_parser::basic b ) {
  return b.get< idn >( 1 ).print();
}

//------------------------------------//
// Arrays                             //
//------------------------------------//

VT nts_2_Nts::arr_size_from_array( nts_parser::nts_array a, NamMap& scope ) {
  return map_replace< VT >( [&] (auto r) {
      return this->term_from_arith_term( deitemise< AT >( r ), scope ).release();
    },
    a.flat, a.flat.begin() + 1, a.flat.end() );
}

VT nts_2_Nts::arr_size_from_array_ref( nts_parser::array_ref ar, NamMap& scope ) {
  using K = nts_parser::array_ref::kind;
  
  switch ( ar.flat[ 0 ] ) {
  case K::ref_basic : return VT();
  case K::ref_array :
    return arr_size_from_array( ar.get< nts_parser::nts_array >( 1 ), scope );
  default : assert( false ); return VT();
  }
}

//------------------------------------//
// Terms                              //
//------------------------------------//

uT nts_2_Nts::term_from_index_term( nts_parser::index_term it, NamMap& scope ) {
  using IT = nts_parser::index_term;
  using K = IT::kind;
  
  switch ( it.flat[ 0 ] ) {
  case K::literal :
    return term_from_arith_lit( it.get< nts_parser::arith_lit >( 1 ), scope );
  case K::array :
    return uT(
      new nts::ArrayTerm(
        term_from_idn( it.get< idn >( 1 ), scope ),
        map_replace< VT >(
          [&] (auto r) {
            auto uptr = this->term_from_index_term( deitemise< IT >( r ), scope );
            return uptr.release();
          },
          it.flat, it.flat.begin() + 1, it.flat.end()
        )
      )
    );
  case K::parenthesised :
    return term_from_index_term( it.get< IT >( 1 ), scope );
  case K::function :
    return uT(
      new nts::ArithmeticOperation(
        arithop_from_aop( it.get< nts_parser::aop >( 2 ) ),
        term_from_index_term( it.get< IT >( 1 ), scope ),
        term_from_index_term( it.get< IT >( 3 ), scope )
      )
    );
  default : assert( false ); return uT();
  }
}

//TODO:signs
uT nts_2_Nts::term_from_arith_term( AT at, NamMap& scope ) {
  using K = AT::kind;
  
  switch ( at.flat[ 0 ] ) {
  case K::literal :
    return term_from_arith_lit( at.get< nts_parser::arith_lit >( 2 ), scope );
  case K::array :
    return term_from_array_term( at.get< nts_parser::array_term >( 2 ), scope );
  case K::parenthesised :
    return term_from_arith_term( at.get< AT >( 1 ), scope );
  case K::function :
    return uT(
      new nts::ArithmeticOperation(
        arithop_from_aop( at.get< nts_parser::aop >( 2 ) ),
        term_from_arith_term( at.get< AT >( 1 ), scope ),
        term_from_arith_term( at.get< AT >( 3 ), scope )
      )
    );
  default : assert( false ); return uT();
  }
}

uT nts_2_Nts::term_from_array_term( nts_parser::array_term at, NamMap& scope ) {
  if ( at.flat[ 0 ] == nts_parser::array_term::kind::array ) {
    auto f = [&] (auto e) { return this->term_from_array_read( e, scope ); };
    return nts_trans< nts_parser::array_read >( f, at, 1 );
  }
  else {
    auto f = [&] (auto e) { return this->term_from_idn( e, scope ); };
    return uT( new nts::ArrayTerm( nts_trans< idn >( f, at, 1 ), {} ) );
  }
}

uT nts_2_Nts::term_from_array_read( nts_parser::array_read ar, NamMap& scope ) {
  using IT = nts_parser::index_term;

  return uT(
    new nts::ArrayTerm(
      term_from_idn( ar.get< idn >( 0 ), scope ),
      map_replace< VT >(
        [&] (auto r) {
          auto uptr = this->term_from_index_term( deitemise< IT >( r ), scope );
          return uptr.release();
        },
        ar.flat, ar.flat.begin() + 1, ar.flat.end()
      )
    )
  );
}

uT nts_2_Nts::term_from_idn( idn i, NamMap& scope ) {
  return uT(
    new nts::VariableReference(
      *lookup_variable( i.print(), scope ),
      false
    )
  );
}

uT nts_2_Nts::term_from_arith_lit( nts_parser::arith_lit al, NamMap& scope ) {
  using DT = nts::DataType;
  using ST = nts::ScalarType;
  using K = nts_parser::arith_lit::kind;
  
  switch( al.flat[ 0 ] ) {
  case K::arith_id :
    return term_from_id( al.get< nts_parser::id >( 1 ), scope );
  case K::arith_tid :
    return uT( new nts::ThreadID() );
  case K::arith_num :
    return uT(
      new nts::UserConstant(
        DT( ST::Integer() ),
        al.get< nts_parser::numeral >().print()
      )
    );
  case K::arith_bool :
    return uT(
      new nts::UserConstant(
        DT( ST::BitVector( 1 ) ),
        al.get< nts_parser::boolean >().print()
      )
    );
  case K::arith_dec :
    return uT(
      new nts::UserConstant(
        DT( ST::Real() ),
        al.get< nts_parser::decimal >().print()
      )
    );
  default : assert( false ); return uT();
  }
}
