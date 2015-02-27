# nts-parser
C++ parser for the Numerical Transition System language

In the original NTS language, the following arithmetic term is ungramatical:

a[i+1]

Instead, we have modified the syntax for arithmetic terms (<index-term> is added
and <array-read> is modified).

<index-term> ::= <arith-lit>
               | <idn> [ <index-term> ]
               | <index-term> <aop> <index-term>
               | ( <index-term> )
               | <index-term> [ <index-term> ]

<array-read> ::= <idn> [ <index-term> ]
               | <array-read> [ <index-term> ]