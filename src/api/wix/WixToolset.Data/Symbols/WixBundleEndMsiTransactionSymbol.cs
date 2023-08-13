// Copyright (c) .NET Foundation and contributors. All rights reserved. Licensed under the Microsoft Reciprocal License. See LICENSE.TXT file in the project root for full license information.

namespace WixToolset.Data
{
    using WixToolset.Data.Symbols;

    public static partial class SymbolDefinitions
    {
        public static readonly IntermediateSymbolDefinition WixBundleEndMsiTransaction = new IntermediateSymbolDefinition(
            SymbolDefinitionType.WixBundleEndMsiTransaction,
            new[]
            {
                new IntermediateFieldDefinition(nameof(WixBundleEndMsiTransactionSymbolFields.TransactionId), IntermediateFieldType.String),
            },
            typeof(WixBundleEndMsiTransactionSymbol));
    }
}

namespace WixToolset.Data.Symbols
{
    using System;

    public enum WixBundleEndMsiTransactionSymbolFields
    {
        TransactionId,
    }

    public class WixBundleEndMsiTransactionSymbol : IntermediateSymbol
    {
        public WixBundleEndMsiTransactionSymbol() : base(SymbolDefinitions.WixBundleEndMsiTransaction, null, null)
        {
        }

        public WixBundleEndMsiTransactionSymbol(SourceLineNumber sourceLineNumber, Identifier id = null) : base(SymbolDefinitions.WixBundleEndMsiTransaction, sourceLineNumber, id)
        {
        }

        public IntermediateField this[WixBundleEndMsiTransactionSymbolFields index] => this.Fields[(int)index];

        public string TransactionId
        {
            get => (string)this.Fields[(int)WixBundleEndMsiTransactionSymbolFields.TransactionId];
            set => this.Set((int)WixBundleEndMsiTransactionSymbolFields.TransactionId, value);
        }
    }
}
