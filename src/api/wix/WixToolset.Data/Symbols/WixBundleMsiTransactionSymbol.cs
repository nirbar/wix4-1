// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.

namespace WixToolset.Data
{
    using WixToolset.Data.Symbols;

    public static partial class SymbolDefinitions
    {
        public static readonly IntermediateSymbolDefinition WixBundleMsiTransaction = new IntermediateSymbolDefinition(
            SymbolDefinitionType.WixBundleMsiTransaction,
            new[]
            {
                new IntermediateFieldDefinition(nameof(WixBundleMsiTransactionSymbolFields.LogPathVariable), IntermediateFieldType.String),
            },
            typeof(WixBundleMsiTransactionSymbol));
    }
}

namespace WixToolset.Data.Symbols
{
    using System;

    public enum WixBundleMsiTransactionSymbolFields
    {
        LogPathVariable,
    }

    public class WixBundleMsiTransactionSymbol : IntermediateSymbol
    {
        public WixBundleMsiTransactionSymbol() : base(SymbolDefinitions.WixBundleMsiTransaction, null, null)
        {
        }

        public WixBundleMsiTransactionSymbol(SourceLineNumber sourceLineNumber, Identifier id = null) : base(SymbolDefinitions.WixBundleMsiTransaction, sourceLineNumber, id)
        {
        }

        public IntermediateField this[WixBundleMsiTransactionSymbolFields index] => this.Fields[(int)index];

        public string LogPathVariable
        {
            get => (string)this.Fields[(int)WixBundleMsiTransactionSymbolFields.LogPathVariable];
            set => this.Set((int)WixBundleMsiTransactionSymbolFields.LogPathVariable, value);
        }
    }
}
