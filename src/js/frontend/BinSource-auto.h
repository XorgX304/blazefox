// This file was autogenerated by binjs_generate_spidermonkey,
// please DO NOT EDIT BY HAND.
/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
* vim: set ts=8 sts=4 et sw=4 tw=99:
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// To generate this file, see the documentation in
// js/src/frontend/binsource/README.md.

// This file is meant to be included from the declaration
// of class `BinASTParser`. The include may be public or private.


// ----- Declaring string enums (by lexicographical order)
enum class AssertedDeclaredKind {
    Var                       /* "var" */,
    NonConstLexical           /* "non-const lexical" */,
    ConstLexical              /* "const lexical" */
};

enum class BinaryOperator {
    Comma                     /* "," */,
    LogicalOr                 /* "||" */,
    LogicalAnd                /* "&&" */,
    BitOr                     /* "|" */,
    BitXor                    /* "^" */,
    BitAnd                    /* "&" */,
    Eq                        /* "==" */,
    Neq                       /* "!=" */,
    StrictEq                  /* "===" */,
    StrictNeq                 /* "!==" */,
    LessThan                  /* "<" */,
    LeqThan                   /* "<=" */,
    GreaterThan               /* ">" */,
    GeqThan                   /* ">=" */,
    In                        /* "in" */,
    Instanceof                /* "instanceof" */,
    Lsh                       /* "<<" */,
    Rsh                       /* ">>" */,
    Ursh                      /* ">>>" */,
    Plus                      /* "+" */,
    Minus                     /* "-" */,
    Mul                       /* "*" */,
    Div                       /* "/" */,
    Mod                       /* "%" */,
    Pow                       /* "**" */
};

enum class CompoundAssignmentOperator {
    PlusAssign                /* "+=" */,
    MinusAssign               /* "-=" */,
    MulAssign                 /* "*=" */,
    DivAssign                 /* "/=" */,
    ModAssign                 /* "%=" */,
    PowAssign                 /* "**=" */,
    LshAssign                 /* "<<=" */,
    RshAssign                 /* ">>=" */,
    UrshAssign                /* ">>>=" */,
    BitOrAssign               /* "|=" */,
    BitXorAssign              /* "^=" */,
    BitAndAssign              /* "&=" */
};

enum class UnaryOperator {
    Plus                      /* "+" */,
    Minus                     /* "-" */,
    Not                       /* "!" */,
    BitNot                    /* "~" */,
    Typeof                    /* "typeof" */,
    Void                      /* "void" */,
    Delete                    /* "delete" */
};

enum class UpdateOperator {
    Incr                      /* "++" */,
    Decr                      /* "--" */
};

enum class VariableDeclarationKind {
    Var                       /* "var" */,
    Let                       /* "let" */,
    Const                     /* "const" */
};



// ----- Sums of interfaces (by lexicographical order)
// Implementations are autogenerated
// `ParseNode*` may never be nullptr
JS::Result<Ok> parseAssertedMaybePositionalParameterName(
    AssertedScopeKind scopeKind,
    MutableHandle<GCVector<JSAtom*>> positionalParams);
JS::Result<ParseNode*> parseAssignmentTarget();
JS::Result<ParseNode*> parseAssignmentTargetOrAssignmentTargetWithInitializer();
JS::Result<ParseNode*> parseAssignmentTargetProperty();
JS::Result<ParseNode*> parseBinding();
JS::Result<ParseNode*> parseBindingOrBindingWithInitializer();
JS::Result<ParseNode*> parseBindingProperty();
JS::Result<ParseNode*> parseExpression();
JS::Result<ParseNode*> parseExpressionOrSuper();
JS::Result<ParseNode*> parseExpressionOrTemplateElement();
JS::Result<ParseNode*> parseForInOfBindingOrAssignmentTarget();
JS::Result<ParseNode*> parseFunctionDeclarationOrClassDeclarationOrExpression();
JS::Result<ParseNode*> parseFunctionDeclarationOrClassDeclarationOrVariableDeclaration();
JS::Result<ParseNode*> parseImportDeclarationOrExportDeclarationOrStatement();
JS::Result<ParseNode*> parseMethodDefinition();
JS::Result<ParseNode*> parseObjectProperty();
JS::Result<ParseNode*> parseParameter();
JS::Result<ParseNode*> parseProgram();
JS::Result<ParseNode*> parsePropertyName();
JS::Result<ParseNode*> parseSimpleAssignmentTarget();
JS::Result<ParseNode*> parseSpreadElementOrExpression();
JS::Result<ParseNode*> parseStatement();
JS::Result<Ok> parseSumAssertedMaybePositionalParameterName(const size_t start, const BinKind kind, const BinFields& fields,
    AssertedScopeKind scopeKind,
    MutableHandle<GCVector<JSAtom*>> positionalParams);
JS::Result<ParseNode*> parseSumAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumAssignmentTargetOrAssignmentTargetWithInitializer(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumAssignmentTargetProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumBinding(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumBindingOrBindingWithInitializer(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumBindingProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumExpressionOrSuper(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumExpressionOrTemplateElement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumForInOfBindingOrAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumFunctionDeclarationOrClassDeclarationOrExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumFunctionDeclarationOrClassDeclarationOrVariableDeclaration(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumImportDeclarationOrExportDeclarationOrStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumMethodDefinition(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumObjectProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumParameter(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumProgram(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumPropertyName(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumSimpleAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumSpreadElementOrExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseSumVariableDeclarationOrExpression(const size_t start, const BinKind kind, const BinFields& fields);


// ----- Interfaces (by lexicographical order)
// Implementations are autogenerated
// `ParseNode*` may never be nullptr
JS::Result<ParseNode*> parseArrowExpressionContentsWithExpression();
JS::Result<ParseNode*> parseArrowExpressionContentsWithFunctionBody();
JS::Result<Ok> parseAssertedBlockScope();
JS::Result<Ok> parseAssertedBoundName(
    AssertedScopeKind scopeKind);
JS::Result<Ok> parseAssertedBoundNamesScope();
JS::Result<Ok> parseAssertedDeclaredName(
    AssertedScopeKind scopeKind);
JS::Result<Ok> parseAssertedParameterScope(
    MutableHandle<GCVector<JSAtom*>> positionalParams);
JS::Result<Ok> parseAssertedScriptGlobalScope();
JS::Result<Ok> parseAssertedVarScope();
JS::Result<ParseNode*> parseAssignmentTargetIdentifier();
JS::Result<ParseNode*> parseBindingIdentifier();
JS::Result<ParseNode*> parseBlock();
JS::Result<LexicalScopeNode*> parseCatchClause();
JS::Result<ParseNode*> parseClassElement();
JS::Result<ParseNode*> parseDirective();
JS::Result<ParseNode*> parseExportFromSpecifier();
JS::Result<ParseNode*> parseExportLocalSpecifier();
JS::Result<ListNode*> parseFormalParameters();
JS::Result<Ok> parseFunctionExpressionContents(
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<Ok> parseFunctionOrMethodContents(
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<Ok> parseGetterContents(
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<ParseNode*> parseIdentifierExpression();
JS::Result<ParseNode*> parseImportSpecifier();
JS::Result<Ok> parseSetterContents(
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<CaseClause*> parseSwitchCase();
JS::Result<ParseNode*> parseSwitchDefault();
JS::Result<ParseNode*> parseVariableDeclarator();
JS::Result<ParseNode*> parseInterfaceArrayAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceArrayBinding(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceArrayExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceArrowExpressionContentsWithExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceArrowExpressionContentsWithFunctionBody(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceAssertedBlockScope(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceAssertedBoundName(const size_t start, const BinKind kind, const BinFields& fields,
    AssertedScopeKind scopeKind);
JS::Result<Ok> parseInterfaceAssertedBoundNamesScope(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceAssertedDeclaredName(const size_t start, const BinKind kind, const BinFields& fields,
    AssertedScopeKind scopeKind);
JS::Result<ParseNode*> parseInterfaceAssertedParameterName(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceAssertedParameterScope(const size_t start, const BinKind kind, const BinFields& fields,
    MutableHandle<GCVector<JSAtom*>> positionalParams);
JS::Result<Ok> parseInterfaceAssertedPositionalParameterName(const size_t start, const BinKind kind, const BinFields& fields,
    AssertedScopeKind scopeKind,
    MutableHandle<GCVector<JSAtom*>> positionalParams);
JS::Result<ParseNode*> parseInterfaceAssertedRestParameterName(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceAssertedScriptGlobalScope(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceAssertedVarScope(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceAssignmentExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceAssignmentTargetIdentifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceAssignmentTargetPropertyIdentifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceAssignmentTargetPropertyProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceAssignmentTargetWithInitializer(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceAwaitExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBinaryExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBindingIdentifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBindingPropertyIdentifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBindingPropertyProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBindingWithInitializer(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBlock(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceBreakStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceCallExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<LexicalScopeNode*> parseInterfaceCatchClause(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceClassDeclaration(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceClassElement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceClassExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceCompoundAssignmentExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceComputedMemberAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceComputedMemberExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceComputedPropertyName(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceConditionalExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceContinueStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceDataProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceDebuggerStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceDirective(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceDoWhileStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerArrowExpressionWithExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerArrowExpressionWithFunctionBody(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerFunctionDeclaration(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerFunctionExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerGetter(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerMethod(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEagerSetter(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceEmptyStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExport(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExportAllFrom(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExportDefault(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExportFrom(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExportFromSpecifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExportLocalSpecifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExportLocals(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceExpressionStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceForInOfBinding(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceForInStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceForOfStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceForStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ListNode*> parseInterfaceFormalParameters(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceFunctionExpressionContents(const size_t start, const BinKind kind, const BinFields& fields,
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<Ok> parseInterfaceFunctionOrMethodContents(const size_t start, const BinKind kind, const BinFields& fields,
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<Ok> parseInterfaceGetterContents(const size_t start, const BinKind kind, const BinFields& fields,
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<ParseNode*> parseInterfaceIdentifierExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceIfStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceImport(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceImportNamespace(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceImportSpecifier(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLabelledStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazyArrowExpressionWithExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazyArrowExpressionWithFunctionBody(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazyFunctionDeclaration(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazyFunctionExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazyGetter(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazyMethod(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLazySetter(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralBooleanExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralInfinityExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralNullExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralNumericExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralPropertyName(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralRegExpExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceLiteralStringExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceModule(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceNewExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceNewTargetExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceObjectAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceObjectBinding(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceObjectExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceReturnStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceScript(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<Ok> parseInterfaceSetterContents(const size_t start, const BinKind kind, const BinFields& fields,
    uint32_t funLength,
    ListNode** paramsOut,
    ListNode** bodyOut);
JS::Result<ParseNode*> parseInterfaceShorthandProperty(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceSpreadElement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceStaticMemberAssignmentTarget(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceStaticMemberExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceSuper(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<CaseClause*> parseInterfaceSwitchCase(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceSwitchDefault(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceSwitchStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceSwitchStatementWithDefault(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceTemplateElement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceTemplateExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceThisExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceThrowStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceTryCatchStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceTryFinallyStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceUnaryExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceUpdateExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceVariableDeclaration(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceVariableDeclarator(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceWhileStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceWithStatement(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceYieldExpression(const size_t start, const BinKind kind, const BinFields& fields);
JS::Result<ParseNode*> parseInterfaceYieldStarExpression(const size_t start, const BinKind kind, const BinFields& fields);


// ----- String enums (by lexicographical order)
// Implementations are autogenerated
JS::Result<typename BinASTParser<Tok>::AssertedDeclaredKind> parseAssertedDeclaredKind();
JS::Result<typename BinASTParser<Tok>::BinaryOperator> parseBinaryOperator();
JS::Result<typename BinASTParser<Tok>::CompoundAssignmentOperator> parseCompoundAssignmentOperator();
JS::Result<typename BinASTParser<Tok>::UnaryOperator> parseUnaryOperator();
JS::Result<typename BinASTParser<Tok>::UpdateOperator> parseUpdateOperator();
JS::Result<typename BinASTParser<Tok>::VariableDeclarationKind> parseVariableDeclarationKind();


// ----- Lists (by lexicographical order)
// Implementations are autogenerated
JS::Result<ParseNode*> parseArguments();
JS::Result<ListNode*> parseFunctionBody();
JS::Result<Ok> parseListOfAssertedBoundName(
    AssertedScopeKind scopeKind);
JS::Result<Ok> parseListOfAssertedDeclaredName(
    AssertedScopeKind scopeKind);
JS::Result<Ok> parseListOfAssertedMaybePositionalParameterName(
    AssertedScopeKind scopeKind,
    MutableHandle<GCVector<JSAtom*>> positionalParams);
JS::Result<ParseNode*> parseListOfAssignmentTargetOrAssignmentTargetWithInitializer();
JS::Result<ParseNode*> parseListOfAssignmentTargetProperty();
JS::Result<ParseNode*> parseListOfBindingProperty();
JS::Result<ParseNode*> parseListOfClassElement();
JS::Result<ListNode*> parseListOfDirective();
JS::Result<ParseNode*> parseListOfExportFromSpecifier();
JS::Result<ParseNode*> parseListOfExportLocalSpecifier();
JS::Result<ParseNode*> parseListOfExpressionOrTemplateElement();
JS::Result<ParseNode*> parseListOfImportDeclarationOrExportDeclarationOrStatement();
JS::Result<ParseNode*> parseListOfImportSpecifier();
JS::Result<ListNode*> parseListOfObjectProperty();
JS::Result<ParseNode*> parseListOfOptionalBindingOrBindingWithInitializer();
JS::Result<ListNode*> parseListOfOptionalSpreadElementOrExpression();
JS::Result<ListNode*> parseListOfParameter();
JS::Result<ListNode*> parseListOfStatement();
JS::Result<ListNode*> parseListOfSwitchCase();
JS::Result<ListNode*> parseListOfVariableDeclarator();


// ----- Default values (by lexicographical order)
// Implementations are autogenerated
JS::Result<ParseNode*> parseOptionalAssignmentTarget();
JS::Result<ParseNode*> parseOptionalBinding();
JS::Result<ParseNode*> parseOptionalBindingIdentifier();
JS::Result<ParseNode*> parseOptionalBindingOrBindingWithInitializer();
JS::Result<LexicalScopeNode*> parseOptionalCatchClause();
JS::Result<ParseNode*> parseOptionalExpression();
JS::Result<ParseNode*> parseOptionalSpreadElementOrExpression();
JS::Result<ParseNode*> parseOptionalStatement();
JS::Result<ParseNode*> parseOptionalVariableDeclarationOrExpression();

