// -----------------------------------------------------------------------
// <copyright file="Tokenizer.cs" company="">
// TODO: Update copyright text.
// </copyright>
// -----------------------------------------------------------------------

namespace HVMCompiler
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public enum HVMTokenId
    {
        Plus,
        PlusEquals,
        Minus,
        MinusEquals,
        Multiply,
        MultiplyEquals,
        Divide,
        DivideEquals,
        Mod,
        ModEquals,
        And,
        AndEquals,
        Or,
        OrEquals,
        Xor,
        XorEquals,
        Negate,
        ShiftLeft,
        ShiftRight,
        ConditionalAnd,
        ConditionalOr,
        ConditionalEquals,
        Equals,
        LessThan,
        LessThanEqualTo,
        GreaterThan,
        GreaterThanEqualTo,
        NotEqualTo,
        PointerIdentifier,
        DotAccessorIdentifier,
        OpenParenthesis,
        CloseParenthesis,
        TermSeperator,
        OpenSquareBracket,
        CloseSquareBracket,
        StatementTerminator,
        TypeIdentifier,
        VarKeyword,
        AddressKeyword,
        SizeofKeyword,
        StructKeyword,
        WhileKeyword,
        FuncKeyword,
        RetKeyword,
        ForKeyword,
        ToKeyword,
        InKeyword,
        IfKeyword,
        ElseIfKeyword,
        ElseKeyword,
        LiteralIdentifier,
    }

    public class HVMToken
    {
        public string Value;
        public HVMTokenId Id;
        public bool IsKeyword = false;
        public int SourceIndex;
        public string TokenString;
    }

    /// <summary>
    /// Tokenizer for the programming language.
    /// </summary>
    public class Tokenizer
    {

        private static Dictionary<string, HVMToken> _AvailableTokens = new Dictionary<string, HVMToken>() 
        {
            // These are defined in a specific order.
            { "+=", new HVMToken { Id = HVMTokenId.PlusEquals } },
            { "+", new HVMToken { Id = HVMTokenId.Plus } },
            { "-=", new HVMToken { Id = HVMTokenId.MinusEquals } },
            { "-", new HVMToken { Id = HVMTokenId.Minus } },
            { "*=", new HVMToken { Id = HVMTokenId.MultiplyEquals } },
            { "*", new HVMToken { Id = HVMTokenId.Multiply } },
            { "/=", new HVMToken { Id = HVMTokenId.DivideEquals } },
            { "/", new HVMToken { Id = HVMTokenId.Divide } },
            { "%=", new HVMToken { Id = HVMTokenId.ModEquals } },
            { "%", new HVMToken { Id = HVMTokenId.Mod } },
            { "&=", new HVMToken { Id = HVMTokenId.AndEquals } },
            { "&", new HVMToken { Id = HVMTokenId.And } },
            { "|=", new HVMToken { Id = HVMTokenId.OrEquals } },
            { "|", new HVMToken { Id = HVMTokenId.Or } },
            { "^=", new HVMToken { Id = HVMTokenId.XorEquals } },
            { "^", new HVMToken { Id = HVMTokenId.Xor } },
            { "~", new HVMToken { Id = HVMTokenId.Negate } },
            { "&&", new HVMToken { Id = HVMTokenId.ConditionalAnd } },
            { "||", new HVMToken { Id = HVMTokenId.ConditionalOr } },
            { "==", new HVMToken { Id = HVMTokenId.ConditionalEquals } },
            { "=", new HVMToken { Id = HVMTokenId.Equals } },
            { "<<", new HVMToken { Id = HVMTokenId.ShiftLeft } },
            { ">>", new HVMToken { Id = HVMTokenId.ShiftRight } },
            { "<=", new HVMToken { Id = HVMTokenId.LessThanEqualTo } },
            { "<", new HVMToken { Id = HVMTokenId.LessThan } },
            { ">=", new HVMToken { Id = HVMTokenId.GreaterThanEqualTo } },
            { ">", new HVMToken { Id = HVMTokenId.GreaterThan } },
            { "!=", new HVMToken { Id = HVMTokenId.NotEqualTo } },
            { "$", new HVMToken { Id = HVMTokenId.PointerIdentifier } },
            { "(", new HVMToken { Id = HVMTokenId.OpenParenthesis } },
            { ")", new HVMToken { Id = HVMTokenId.CloseParenthesis } },
            { ".", new HVMToken { Id = HVMTokenId.DotAccessorIdentifier } },
            { ",", new HVMToken { Id = HVMTokenId.TermSeperator } },
            { "[", new HVMToken { Id = HVMTokenId.OpenSquareBracket } },
            { "]", new HVMToken { Id = HVMTokenId.CloseSquareBracket } },
            { ";", new HVMToken { Id = HVMTokenId.StatementTerminator } },
            { ":", new HVMToken { Id = HVMTokenId.TypeIdentifier } },
            { "var", new HVMToken { Id = HVMTokenId.VarKeyword, IsKeyword=true } },
            { "address", new HVMToken { Id = HVMTokenId.AddressKeyword, IsKeyword=true } },
            { "sizeof", new HVMToken { Id = HVMTokenId.SizeofKeyword, IsKeyword=true } },
            { "func", new HVMToken { Id = HVMTokenId.FuncKeyword, IsKeyword=true } },
            { "while", new HVMToken { Id = HVMTokenId.WhileKeyword, IsKeyword=true } },
            { "ret", new HVMToken { Id = HVMTokenId.RetKeyword, IsKeyword=true } },
            { "for", new HVMToken { Id = HVMTokenId.ForKeyword, IsKeyword=true } },
            { "to", new HVMToken { Id = HVMTokenId.ToKeyword, IsKeyword=true } },
            
            // Data types, reserved keywords
            { "struct", new HVMToken { Id = HVMTokenId.StructKeyword, IsKeyword=true } },
            //{ "int", new HVMToken { Id = HVMTokenId.IntKeyword, IsKeyword=true } }, // Has to be above in keyword
            //{ "string", new HVMToken { Id = HVMTokenId.StringKeyword, IsKeyword=true } },
            //{ "float", new HVMToken { Id = HVMTokenId.FloatKeyword, IsKeyword=true } },
            
            { "in", new HVMToken { Id = HVMTokenId.InKeyword, IsKeyword=true } },
            { "if", new HVMToken { Id = HVMTokenId.IfKeyword, IsKeyword=true } },
            { "elseif", new HVMToken { Id = HVMTokenId.ElseIfKeyword, IsKeyword=true } },
            { "else", new HVMToken { Id = HVMTokenId.ElseKeyword, IsKeyword=true } },
        };

        private List<HVMToken> _Tokens;

        private int _Index;
        private string _Source;

        public Tokenizer(string source)
        {
            _Source = source;
            _Index = 0;
            _Tokens = new List<HVMToken>();
        }

        public string GetSource() { return _Source; }

        public List<HVMToken> GetTokens()
        {
            _Source = _Source.Replace("\n", "").Replace("\t", "").Replace("\r", "");

            Console.WriteLine(_Source);

            // These are ordered in a specific way.

            while (_Index < _Source.Length)
            {
                bool found = false;
                HVMToken foundToken = null;
                char currentChar = _Source[_Index];

                foreach (KeyValuePair<string, HVMToken> tokens in Tokenizer._AvailableTokens)
                {
                    // Look for keywords and operators in the available tokens.
                    if (ContainsString(tokens.Key, _Index))
                    {
                        // Create the token with the id and stuff.

                        foundToken = new HVMToken();
                        foundToken.Value = tokens.Value.Value;
                        foundToken.Id = tokens.Value.Id;
                        foundToken.IsKeyword = tokens.Value.IsKeyword;
                        foundToken.SourceIndex = _Index;
                        foundToken.TokenString = tokens.Key;

                        _Index += tokens.Key.Length;
                        found = true;

                        break;
                    }
                }

                if (found == false)
                {
                    // Look for some stuff.
                    // So it could be a space, control character that i have missed?
                    // Number, word, etc.
                    if (_Source[_Index] == ' ')
                        _Index++;
                    else // Skip over for now, however will change to read string.
                    {
                        foundToken = new HVMToken();
                        foundToken.Id = HVMTokenId.LiteralIdentifier;
                        foundToken.IsKeyword = false;
                        foundToken.SourceIndex = _Index;
                        string literal = GetString();
                        foundToken.Value = literal;
                    }
                }
                else if (found == true) // Some checking and catching of invalid keyword tokens.
                {
                    // Keywords need to be checked that what the keyword thinks it is
                    // and what it actually is could be different things.
                    if (foundToken.IsKeyword == true)
                    {
                        // This makes you think it needs + 1 however source index is the first character
                        // For this token so adding the length will go to the last+1 character.
                        int peekAheadCharacter = foundToken.TokenString.Length + foundToken.SourceIndex;
                        if (peekAheadCharacter < _Source.Length)
                        {
                            char peekAhead = _Source[peekAheadCharacter];
                            if (char.IsLetterOrDigit(peekAhead) || peekAhead == '_')
                            {
                                _Index = foundToken.SourceIndex;
                                // Read this as a word or it could be a number
                                // Both are literals in this stage of the compiler.
                                foundToken = new HVMToken();
                                foundToken.Id = HVMTokenId.LiteralIdentifier;
                                foundToken.IsKeyword = false;
                                foundToken.SourceIndex = _Index;
                                foundToken.Value = GetString();

                                _Index = foundToken.SourceIndex + foundToken.Value.Length;
                            }
                        }
                    }
                }

                if (foundToken != null)
                {
                    Console.WriteLine(foundToken.Id + " " + foundToken.Value);
                    this._Tokens.Add(foundToken);
                }
            }

            return _Tokens;
        }

        private bool ContainsString(string characters, int offset)
        {
            if (offset+characters.Length <= _Source.Length)
                if (_Source.Substring(offset, characters.Length).Equals(characters))
                    return true;
            return false;
        }

        private string GetString()
        {
            StringBuilder builder = new StringBuilder();

            char c = _Source[_Index];

            while (char.IsLetterOrDigit(c) || c == '_')
            {
                builder.Append(c);
                c = _Source[++_Index];
            }
            
            return builder.ToString();
        }
    }
}
