using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HVMCompiler
{
    class Program
    {
        /*
        Programming Language Design:
		    Strongely typed.
		    Structs.
		    Functions
        
         * Objects are given a type using the format name:type
         * Statement terminators are given by ;
         * 
         * Available data types:
         *  - float
         *  - int
         *  - string
         *  - structs
         *  - pointers
         *  
         * Available Operators:
         * +
         * +=
         * -
         * -=
         * *
         * *=
         * /
         * /=
         * %
         * %=
         * &
         * &=
         * |
         * |=
         * ^
         * ^=
         * ~
         * &&
         * ||
         * ==
         * =
         * <
         * <=
         * >
         * >=
         * !=
         * $ (shows the variable is a pointer for a data type)
         * (
         * )
         * . (member access for struct value, pointers will be accessed by $variable.member1 if variable is declared var $variable:MyStruct)
         * , Term seperator for arrays and lists.
         * [
         * ]
         * ; (End of statement)
         * : (type identifier)
         * Keywords:
         * var
         * address (gets the address of a variable, similar to & in c++, makes it easier to figure out what is what for syntax)
         * sizeof (gets the sizeof a variable)
         * struct
         * func
         * ret
         * for
         * to (used in for loops for ranges of a letter)
         * in
         * if
         * elseif
         * else
           
            func:<return-type> name(<argument>:<type>, ..) (<func-body>)
            struct name(<member>:<type>, ...)
            var name:<data-type>;
            
		    struct MyStruct(
			    a:string,
			    b:int[]
		    )

		    var p:int = 10;
		    p+=12;

		    func:int adder(a:int, b:int) (
			    ret a+b;
		    )

		    func:$MyStruct +($a:MyStruct, $b:MyStruct) {
			    var result:$MyStruct;
                $result.a = $a.a + $b.a;
                $result.b = $a.b + $b.b;
                ret result;
		    }

		    adder(10, 4);

		    for index(0 to p) (
		        print(index); 
            )

		    for (a:int in array) (
		    )

		    while (a<0) (
		    )

		    if (...) (
		    )
		    elseif (
		    )
		    else (
		    )
            */

        static void Main(string[] args)
        {
            /*    
                var r:$vec3;
                $r.x = $a.x + $b.x;
                $r.y = $a.y + $b.y;
                $r.z = $a.z + $b.z;
                return r;
            */
            
            string simpleProgram =
@"
func:$int[] orange(a:int, b:$string[]) (
)

func:$int[] orange() (
)

func:$vec3 +(a:$vec3, b:$vec3) (
)

func nothing() (
)

struct name(a:string[])
var p:test=0;";

            string simple2Program = 
@"
struct($p:int, index:int)
var p:int = 2;
for index(0 to p) (
    p+=4;
    p = p + 1;
    $ptr:int = address(p);
)";

            Tokenizer tokenizer = new Tokenizer(simpleProgram);
            List<HVMToken> Tokens = tokenizer.GetTokens();

            SyntaxTree tree = new SyntaxTree(Tokens, tokenizer.GetSource());

            /*
            HvmEmitter emitter = new HvmEmitter(32);
            int label_start = emitter.label();
            emitter.push(2);
            emitter.push(4);
            emitter.add();
            emitter.push(5);
            emitter.push(2);
            emitter.mul();
            emitter.mul();
            emitter.pop(); // "Clean stack"
            emitter.jmp(label_start);
            emitter.end();

            emitter.complete();*/

            Console.ReadLine();
        }
    }
}
