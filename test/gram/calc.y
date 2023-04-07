#定义终止符
%Integer
%Float
#定义非终止符
&commands
&exp
&term
&number

#定义起点
start:commands

#commands
commands:exp '\n'
commands:commands exp '\n'

#exp
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