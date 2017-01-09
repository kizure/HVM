// -----------------------------------------------------------------------
// <copyright file="SyntaxTree.cs" company="">
// TODO: Update copyright text.
// </copyright>
// -----------------------------------------------------------------------

#define DEBUG

namespace HVMCompiler
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public class SyntaxNode
    {
        public List<SyntaxNode> SubNodes;
        private SyntaxNode _Owner;

        public SyntaxNode(SyntaxNode owner) 
        { 
            this._Owner = owner;
            this.SubNodes = new List<SyntaxNode>(); 
        }

        public SyntaxNode GetOwner()
        {
            return _Owner;
        }
    }

    public class HVMOperatorFunc : HVMFunc // Operator overload
    {
        public HVMOperatorFunc(SyntaxNode owner) : base(owner) { }
        public HVMTokenId? Operator;

        public new string ToString()
        {
            return "overload func " + this.Operator + " returns " + this.ReturnType.ToString() + " params=" + this.Parameters.Count;
        }
    }

    public class HVMIf : SyntaxNode
    {
        public HVMIf(SyntaxNode owner) : base(owner) { }

        public SyntaxNode ConditionBlock;
        public SyntaxNode
    }

    public class HVMVariable : SyntaxNode
    {
        public HVMVariable(SyntaxNode owner) : base(owner) { }
        public string Name;
        public HVMType VarType;
    }

    public class HVMFunc : SyntaxNode
    {
        public HVMFunc(SyntaxNode owner) : base(owner) { this.Parameters = new List<HVMVariable>(); }
        public string Name;
        public HVMType ReturnType; // Can be null
        public List<HVMVariable> Parameters;
        public SyntaxNode FunctionBody; // The code of the function is in here.

        public override string ToString()
        {
            return "func " + this.Name + " returns " + this.ReturnType.ToString() + " params=" + Parameters.Count;
        }
    }

    public class HVMType
    {
        public bool IsPointer = false;
        public bool IsArray = false;
        public HVMTypes Type;
        public string Name;

        public override string ToString()
        {
            return Name + "[array=" + IsArray + ", pointer=" + IsPointer + ", type=" + Type + "]";
        }
    }

    public class HVMStruct : SyntaxNode
    {
        public HVMStruct(SyntaxNode owner) : base(owner) { this.Parameters = new List<HVMVariable>(); }
        public string Name;
        public List<HVMVariable> Parameters;
    }

    public enum HVMTypes
    {
        String,
        Int,
        Float,
        Array,
        Struct,
        Void,
    }

    /// <summary>
    /// Syntax tree for the code.
    /// </summary>
    public class SyntaxTree
    {
        /// <summary>
        /// Only these operators can be overloaded. List to check against.
        /// </summary>
        private static HVMTokenId[] _OverloadableOperations = new HVMTokenId[] 
        {        
            HVMTokenId.Plus,
            HVMTokenId.Minus,
            HVMTokenId.Multiply,
            HVMTokenId.Divide,
            HVMTokenId.Mod,
            HVMTokenId.And,
            HVMTokenId.Or,
            HVMTokenId.Xor,
            HVMTokenId.Negate,
            HVMTokenId.ConditionalEquals,    
            HVMTokenId.LessThan,
            HVMTokenId.LessThanEqualTo,
            HVMTokenId.GreaterThan,
            HVMTokenId.GreaterThanEqualTo,
            HVMTokenId.NotEqualTo,
            HVMTokenId.ShiftLeft,
            HVMTokenId.ShiftRight,
        };

        private List<HVMToken> _Tokens;
        private string _Source;

        private int _Index = 0;

        private SyntaxNode _ProgramNode;

        public SyntaxTree(List<HVMToken> Tokens, string source)
        {
            _ProgramNode = new SyntaxNode(null);
            this._Tokens = Tokens;
            this._Source = source;
            // Create tree from tokens.

            HVMToken token;
            while ((token = GetNextToken()) != null)
            {
                if (token.Id == HVMTokenId.FuncKeyword)
                {
                    // The following forms of functions we expect.
                    // func:<$>type name(argument-block) (code-block)
                    // func name(argument-block) (code-block) - this has no return type (void)
                    // func:type operator(argument-block) (code-block) - does this HAVE to have a type for all overloaddable operators?

                    HVMType type = null;

                    // It has a type identifier.
                    if (PeekNextToken().Id == HVMTokenId.TypeIdentifier)
                        type = GetDataType();
                    else // No type, assume void
                    {
                        type = new HVMType();
                        type.Type = HVMTypes.Void;
                    }
                    
                    // No type identifier means it doesn't return anything.

                    // This could also be an overloaddable operator too.
                    HVMToken nameOrOperator = GetNextToken();

                    HVMFunc funcSignature = null; // Technically the owner is the program body.

                    if (nameOrOperator.Id == HVMTokenId.LiteralIdentifier)
                    {
                        funcSignature = new HVMFunc(_ProgramNode);
                        funcSignature.Name = nameOrOperator.Value;
                    }
                    else
                    {
                        funcSignature = new HVMOperatorFunc(_ProgramNode); 
                        foreach (HVMTokenId overloadable in _OverloadableOperations)
                        {
                            if (nameOrOperator.Id == overloadable)
                            {
                                (funcSignature as HVMOperatorFunc).Operator = nameOrOperator.Id;
                                break;
                            }
                        }
                        if ((funcSignature as HVMOperatorFunc).Operator == null)
                            ThrowSyntaxParseError(nameOrOperator, "Cannot overload that operator ");
                    }

                    funcSignature.ReturnType = type;
                    
                    // Start checking for parameters.
                    GetNextTokenWithTypeCheck(HVMTokenId.OpenParenthesis);

                    if (PeekNextToken().Id == HVMTokenId.CloseParenthesis)
                    {
                        GetNextTokenWithTypeCheck(HVMTokenId.CloseParenthesis);
                        // No parameters in the function.
                        funcSignature.Parameters.Clear(); // Make sure it is empty.
                    }
                    else if (PeekNextToken().Id == HVMTokenId.LiteralIdentifier) // Parameters.
                    {
                        do
                        {
                            // Swollow the term seperator.
                            if (PeekNextToken().Id == HVMTokenId.TermSeperator)
                                GetNextTokenWithTypeCheck(HVMTokenId.TermSeperator);
                            // Get the parameter name and type.
                            HVMToken paramName = GetNextTokenWithTypeCheck(HVMTokenId.LiteralIdentifier);
                            HVMType paramType = GetDataType();

                            // Function owns the parameters.
                            HVMVariable parameter = new HVMVariable(funcSignature);
                            parameter.Name = paramName.Value;
                            parameter.VarType = paramType;
                            funcSignature.Parameters.Add(parameter);
                        } while (PeekNextToken().Id == HVMTokenId.TermSeperator);
                        GetNextTokenWithTypeCheck(HVMTokenId.CloseParenthesis);
                    }
                    else
                    {
                        HVMToken wrongToken = GetNextToken();
                        ThrowSyntaxParseError(wrongToken, "Unexpected token in function signature " + wrongToken.Id + ", expected either parameters or ) ");
                    }

                    funcSignature.FunctionBody = new SyntaxNode(funcSignature);
                    // Main function body code, currently not parsing.
                    GetNextTokenWithTypeCheck(HVMTokenId.OpenParenthesis);
                    GetNextTokenWithTypeCheck(HVMTokenId.CloseParenthesis);

#if DEBUG
                    if (funcSignature is HVMOperatorFunc)
                        Console.WriteLine((funcSignature as HVMOperatorFunc).ToString());
                    else
                        Console.WriteLine(funcSignature.ToString());
#endif
                    // Add the function to the program.
                    _ProgramNode.SubNodes.Add(funcSignature);

                }
                else if (token.Id == HVMTokenId.StructKeyword) // Parse struct.
                {
                    HVMStruct newStruct = new HVMStruct(_ProgramNode);

                    HVMToken name = GetNextTokenWithTypeCheck(HVMTokenId.LiteralIdentifier);
                    newStruct.Name = name.Value;
                    GetNextTokenWithTypeCheck(HVMTokenId.OpenParenthesis);
                    // The members of the struct.
                    do
                    {
                        // Swollow the term seperator.
                        if (PeekNextToken().Id == HVMTokenId.TermSeperator)
                            GetNextTokenWithTypeCheck(HVMTokenId.TermSeperator);
                        // Get the parameter name and type.
                        HVMToken paramName = GetNextTokenWithTypeCheck(HVMTokenId.LiteralIdentifier);
                        HVMType paramType = GetDataType();

                        // Function owns the parameters.
                        HVMVariable parameter = new HVMVariable(newStruct);
                        parameter.Name = paramName.Value;
                        parameter.VarType = paramType;
                        newStruct.Parameters.Add(parameter);
                    } while (PeekNextToken().Id == HVMTokenId.TermSeperator);

                    GetNextTokenWithTypeCheck(HVMTokenId.CloseParenthesis);

                    Console.WriteLine("Struct name=" + newStruct.Name + " parameters=" + newStruct.Parameters.Count);

                    _ProgramNode.SubNodes.Add(newStruct);
                }
            }
        }

        private SyntaxNode GetStatement(SyntaxNode owner)
        {
            // Gets the next statement in the tokenized program.
            HVMToken token = PeekNextToken();

            if (token.Id == HVMTokenId.IfKeyword)
            {

            }
            
            
            return null;
        }

        private HVMVariable GetVariable(SyntaxNode owner)
        {
            GetNextTokenWithTypeCheck(HVMTokenId.VarKeyword);
            HVMToken name = GetNextTokenWithTypeCheck(HVMTokenId.LiteralIdentifier);
            HVMType varType = GetDataType();
            HVMVariable var = new HVMVariable(owner);
            var.VarType = varType;
            var.Name = name.Value;
            return var;
        }

        /// <summary>
        /// Gets the data-type after a type identifier token.
        /// </summary>
        /// <returns>All the information about the data type (pointer, array, type, name)</returns>
        public HVMType GetDataType()
        {
            GetNextTokenWithTypeCheck(HVMTokenId.TypeIdentifier);
            HVMType dataType = new HVMType();
            // Check for pointer label
            HVMToken token = GetNextToken();
            if (token.Id == HVMTokenId.PointerIdentifier) 
            {
                dataType.IsPointer = true; // Mark as pointer
                token = GetNextToken();
            }
            else // Not a pointer
                dataType.IsPointer = false;

            // Make sure it is what we expect it to be.
            if (token.Id != HVMTokenId.LiteralIdentifier)
                ThrowSyntaxParseError(token, "Expected data type got " + token.Id + " at ");

            string dataTypeRaw = token.Value.ToLower();

            if (dataTypeRaw.Equals("int"))
            {
                dataType.Type = HVMTypes.Int;
            }
            else if (dataTypeRaw.Equals("string"))
            {
                dataType.Type = HVMTypes.String;
            }
            else if (dataTypeRaw.Equals("float"))
            {
                dataType.Type = HVMTypes.Float;
            }
            else // Assume it is meaning a user defined data type at the moment. (Cannot validate yet as it is a multi-stage process.)
            {
                dataType.Type = HVMTypes.Struct;
                dataType.Name = token.Value;
            }

            // Check if it is an array. (:$int[] is the most complex format basically.) 
            if (PeekNextToken().Id == HVMTokenId.OpenSquareBracket)
            {
                GetNextTokenWithTypeCheck(HVMTokenId.OpenSquareBracket);
                GetNextTokenWithTypeCheck(HVMTokenId.CloseSquareBracket);
                dataType.IsArray = true;
            }

            return dataType;
        }

        public HVMToken GetNextToken()
        {
            if (_Index >= _Tokens.Count)
                return null;
            return _Tokens[_Index++];
        }

        public HVMToken PeekNextToken()
        {
            if (_Index >= _Tokens.Count)
                return null;
            return _Tokens[_Index];
        }

        public HVMToken GetNextTokenWithTypeCheck(HVMTokenId id)
        {
            HVMToken token = GetNextToken();

            if (token != null)
            {
                if (token.Id != id)
                    ThrowSyntaxParseError(token, "Expected " + id + " got " + token.Id + " at ");
            }
            else
                ThrowSyntaxParseError(null, "Expected " + id + " and got nothing!");

            return token;
        }

        public void ThrowSyntaxParseError(HVMToken token, string message)
        {
            if (token == null) { throw new Exception(message); }
            else
            {
                string sourceSample = "";

                if (token.SourceIndex + 10 < _Source.Length)
                {
                    sourceSample = _Source.Substring(token.SourceIndex, 10);
                }
                else
                {
                    sourceSample = _Source.Substring(token.SourceIndex, _Source.Length - token.SourceIndex);
                }

                int pos = sourceSample.IndexOf(_Source[token.SourceIndex]);
                sourceSample = sourceSample.Insert(pos, "[");
                sourceSample = sourceSample.Insert(pos + 2, "]");

                throw new Exception(message + " " + sourceSample + "...");
            }
        }
    }
}
