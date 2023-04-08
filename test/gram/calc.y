start           :: commands

commands        ::= { exp "\n" }

exp:term
exp:exp + term
exp:exp - term

#term
term:number
term:term * number
term:term / number

#number
number:Integer
number:Float