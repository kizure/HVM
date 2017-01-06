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
    }

    /// <summary>
    /// Tokenizer for the language.
    /// </summary>
    public class Tokenizer
    {

        private static Dictionary<string, HVMToken> _AvailableTokens = new Dictionary<string, HVMToken>() 
        {
            { "+", new HVMToken { Id = HVMTokenId.Plus } },
            { "+=", new HVMToken { Id = HVMTokenId.PlusEquals } },
            { "-", new HVMToken { Id = HVMTokenId.Minus } },
            { "-=", new HVMToken { Id = HVMTokenId.MinusEquals } },
            { "*", new HVMToken { Id = HVMTokenId.Multiply } },
            { "*=", new HVMToken { Id = HVMTokenId.MultiplyEquals } },
            { "/", new HVMToken { Id = HVMTokenId.Divide } },
            { "/=", new HVMToken { Id = HVMTokenId.DivideEquals } },
            { "%", new HVMToken { Id = HVMTokenId.Mod } },
            { "%=", new HVMToken { Id = HVMTokenId.ModEquals } },
            { "&", new HVMToken { Id = HVMTokenId.And } },
            { "&=", new HVMToken { Id = HVMTokenId.AndEquals } },
            { "|", new HVMToken { Id = HVMTokenId.Or } },
            { "|=", new HVMToken { Id = HVMTokenId.OrEquals } },
            { "^", new HVMToken { Id = HVMTokenId.Xor } },
            { "^=", new HVMToken { Id = HVMTokenId.XorEquals } },
            { "~", new HVMToken { Id = HVMTokenId.Negate } },
            { "&&", new HVMToken { Id = HVMTokenId.ConditionalAnd } },
            { "||", new HVMToken { Id = HVMTokenId.ConditionalOr } },
            { "==", new HVMToken { Id = HVMTokenId.ConditionalEquals } },
            { "=", new HVMToken { Id = HVMTokenId.Equals } },
            { "<", new HVMToken { Id = HVMTokenId.LessThan } },
            { "<=", new HVMToken { Id = HVMTokenId.LessThanEqualTo } },
            { ">", new HVMToken { Id = HVMTokenId.GreaterThan } },
            { ">=", new HVMToken { Id = HVMTokenId.GreaterThanEqualTo } },
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
            { "var", new HVMToken { Id = HVMTokenId.VarKeyword } },
            { "address", new HVMToken { Id = HVMTokenId.AddressKeyword } },
            { "sizeof", new HVMToken { Id = HVMTokenId.SizeofKeyword } },
            { "struct", new HVMToken { Id = HVMTokenId.StructKeyword } },
            { "func", new HVMToken { Id = HVMTokenId.FuncKeyword } },
            { "while", new HVMToken { Id = HVMTokenId.WhileKeyword } },
            { "ret", new HVMToken { Id = HVMTokenId.RetKeyword } },
            { "for", new HVMToken { Id = HVMTokenId.ForKeyword } },
            { "to", new HVMToken { Id = HVMTokenId.ToKeyword } },
            { "in", new HVMToken { Id = HVMTokenId.InKeyword } },
            { "if", new HVMToken { Id = HVMTokenId.IfKeyword } },
            { "elseif", new HVMToken { Id = HVMTokenId.ElseIfKeyword } },
            { "else", new HVMToken { Id = HVMTokenId.ElseKeyword } },
            // Will have to be careful with the "" for the key.
            // The literal identifier is for identifiers, struct name, variable names, function names, types, etc
            { "", new HVMToken { Id = HVMTokenId.LiteralIdentifier } },
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

        public List<HVMToken> GetTokens()
        {
            _Source = _Source.Replace("\n", "").Replace("\t", "");

            Console.WriteLine(_Source);

            return _Tokens;
        }
    }
}
