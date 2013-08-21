SemanticChecker
===============

The input to the semantic checker will be a program and the output will be information about the types of the variables in the program or an  error message if there is type mismatch or other semantic error.
The syntax for type declarations is given by the 
following grammar:

   <br>(3)  type_decl_section -> TYPE type_decl_list 
   <br>(4)  type_decl_section ->                                    // epsilon 
   <br>(5)  type_decl_list    -> type_decl type_decl_list
   <br>(6)  type_decl_list    -> type_decl
   <br>(7)  type_decl         -> id_list COLON type_name SEMICOLON
   <br>(8)  type_name         -> REAL
   <br>(9)  type_name         -> INT
   <br>(10) type_name         -> BOOLEAN
   <br>(11) type_name         -> STRING
   <br>(12) type_name         -> ID

<br><br>The language allows the explicit declaration of variables. This is done
in a var_decl_section whose syntax is defined as follows:

   <br>(13) var_decl_section  -> VAR var_decl_list
   <br>(14) var_decl_section  ->                                   // epsilon
   <br>(15) var_decl_list     -> var_decl var_decl_list
   <br>(16) var_decl_list     -> var_decl
   <br>(17) var_decl          -> id_list COLON type_name SEMICOLON
   <br>(18) id_list           -> ID COMMA id_list
   <br>(19) id_list           -> ID
   
<br><br>The body of the program consists of a list of statements between two braces.
The grammar rules are:

   <br>(20) body              -> LBRACE stmt_list RBRACE
   <br>(21) stmt_list         -> stmt stmt_list
   <br>(22) stmt_list         -> stmt
   
<br><br>A statement can be either an assignment (assign_stmt) or a while statement
(while_stmt). An assign_stmt assigns an expression to a variable. A while
statement has 3 parts: (1) the WHILE keyword, (2) a condition, and (3) a body
(this is a recursive definition). The grammar rules for stmt are the following:

   <br>(23) stmt              -> while_stmt
   <br>(24) stmt              -> assign_stmt
   <br>(25) while_stmt        -> WHILE condition body
   <br>(26) assign_stmt       -> ID EQUAL expr SEMICOLON
   <br>(27) expr              -> term PLUS expr
   <br>(28) expr              -> term MINUS expr
   <br>(29) expr              -> term
   <br>(30) term              -> factor MULT term
   <br>(31) term              -> factor DIV term
   <br>(32) term              -> factor
   <br>(33) factor            -> LPAREN expr RPAREN
   <br>(34) factor            -> NUM
   <br>(35) factor            -> REALNUM
   <br>(36) factor			  -> ID
   <br>(37) condition         -> ID                          
   <br>(38) condition         -> primary relop primary
   <br>(39) primary           -> ID
   <br>(40) primary           -> NUM
   <br>(41) primary           -> REALNUM
   <br>(42) relop             -> GREATER
   <br>(43) relop             -> GTEQ
   <br>(44) relop             -> LESS
   <br>(45) relop             -> NOTEQUAL
   <br>(46) relop             -> LTEQ
   
<br><br>We have the following main rules (MR) to be enforced:

    <br>(MR1)  Structural equivalence is enforced
    <br>(MR2)  Each variable (wether or not it is explicitly declared) has a 
           fixed type that does not change
    <br>(MR3)  Each type name (wether or not it is explicitly declared) is fixed 
           and does not change 
    <br>(MR4)  Each type is assigned an integer 
    <br>(MR5)  INT is assigned 10, REAL is assigned 11, STRING is assigned 12, and 
           BOOLEAN is assigned 13. 
    <br>(MR6)  If a : b; is a type declaration, then #a = #b, where #x is the 
           number assigned to x.
    <br>(MR7)  Types that are implicitly declared are assigned unique numbers 
           when they are introduced. The numbers should be greater than 13.
    <br>(MR8)  Each variable is assigned an integer (type number)
    <br>(MR9)  If a : t; is a variable declaration, then #a = #t, where #x is 
           the number assigned to x.
    <br>(MR10) Variables that are implicitly declared are assigned unique numbers 
           when they are introduced. The numbers should be greater than 13.
