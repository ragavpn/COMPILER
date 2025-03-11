In attribute grammars, we have:
1. **S-attributed grammars**: Only synthesized attributes are used, which flow information up the parse tree (from children to parents).
2. **L-attributed grammars**: Both synthesized attributes and inherited attributes are used, but with constraints that ensure attributes can be evaluated in a left-to-right, depth-first traversal.

### Attributes in the Parser

1. **Node values**: The `value` field in the `Node` struct represents a synthesized attribute, as it's computed from children and passed up to parents.

2. **Type information**: The `type` field in nodes is another synthesized attribute - it's determined based on operands and operations.

3. **Symbol table entries**: The symbol table maintains information about variables, including scope, type, and value. This can be considered a side-effect rather than a direct attribute grammar feature.

4. **Function information**: The parser tracks function signatures, parameters, and return types for overload resolution.


```c
expression:
    expression PLUS expression {
        // If either operand is float, result must be float
        if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
            $$ = createNode("+", "float", $1, $3);
            $$->value = (int)($1->value + $3->value);
        } else {
            $$ = createNode("+", "int", $1, $3);
            $$->value = evaluateExpr("+", $1->value, $3->value);
        }
    }
```

Here, the `type` and `value` of the parent node are synthesized from the children's attributes. This pattern is consistent throughout the parser.

However, there are elements like scope management that behave more like inherited attributes:

```c
block:
    LBRACE { currentScope++; } statements RBRACE { currentScope--; $$ = $3; }
```

Here, scope is effectively passed down to children as an inherited attribute. The `currentScope` variable is a global that's modified to implement this inheritance.

So, our grammar appears to be L-attributed, as it has both synthesized attributes (values, types) flowing up and inherited attributes (scope) flowing down. The scope is managed in a way that ensures left-to-right evaluation works properly.

### Semantic Rules

The key semantic rules implemented in our parser include:

1. **Type checking and type coercion**: Expressions have types, and operators enforce type compatibility. For example, if either operand of an arithmetic operation is a float, the result is a float.

2. **Scope management**: Variables are declared in scopes, and lookups respect scope hierarchy.

3. **Variable declaration**: Variables must be declared before use and cannot be redeclared in the same scope.

4. **Function overloading**: Functions can be overloaded based on parameter types.

5. **Value computation**: Expressions are evaluated to produce values.

# Parser Analysis: Attribute Grammar Type and Semantic Rules

## Types of Attributes Used

Our parser implements a **mixed-attribute grammar** that leans heavily toward being an **L-attributed grammar** because it uses both synthesized and inherited attributes with evaluations that proceed in a left-to-right, depth-first manner.

### Synthesized Attributes

The main synthesized attributes in our parser include:

1. **Node values (`value` field)**: Computed from children nodes and passed upward in the parse tree.
   ```c
   $$->value = evaluateExpr("+", $1->value, $3->value);
   ```

2. **Type information (`type` field)**: Determined based on operands and operations, propagated upward.
   ```c
   if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
       $$ = createNode("+", "float", $1, $3);
   } else {
       $$ = createNode("+", "int", $1, $3);
   }
   ```

### Inherited Attributes

1. **Scope information**: The `currentScope` variable is effectively an inherited attribute that flows from parent nodes to children.
   ```c
   block:
       LBRACE { currentScope++; } statements RBRACE { currentScope--; $$ = $3; }
   ```

2. **Function context**: When processing function bodies, parameter information is inherited from the function declaration.

## L-attributed vs S-attributed Analysis

This grammar is **L-attributed** rather than S-attributed because:

1. It uses inherited attributes (scope, function context) that flow top-down.
2. The evaluation order matters and is left-to-right (critical for scope management).
3. Information about earlier siblings affects the processing of later siblings.

An S-attributed grammar would only use synthesized attributes flowing bottom-up, which would be insufficient for our language's scope and context-sensitive features.

## Semantic Rules and Actions

Here are the key semantic rules implemented in our parser:

### 1. Type Checking and Coercion

```
Rule: expression → expression + expression
Action: {
    if (either_is_float(e1, e2))
        e.type = "float";
        e.value = float_value(e1) + float_value(e2);
    else
        e.type = "int";
        e.value = e1.value + e2.value;
}
```

Implementation:
```c
if (strcmp($1->type, "float") == 0 || strcmp($3->type, "float") == 0) {
    $$ = createNode("+", "float", $1, $3);
    $$->value = (int)($1->value + $3->value);
} else {
    $$ = createNode("+", "int", $1, $3);
    $$->value = evaluateExpr("+", $1->value, $3->value);
}
```

### 2. Scope Management

```
Rule: block → { statements }
Action: {
    before statements: currentScope++;
    after statements: currentScope--;
    block.node = statements.node;
}
```

Implementation:
```c
block:
    LBRACE { currentScope++; } statements RBRACE { currentScope--; $$ = $3; }
```

### 3. Variable Declaration and Type Checking

```
Rule: declaration → type identifier = expression ;
Action: {
    check_compatible_types(type, expression.type);
    addSymbol(identifier, type, currentScope, 0);
    declaration.node = new AssignmentNode(identifier, expression);
    update_symbol_value(identifier, expression.value);
}
```

Implementation:
```c
type IDENTIFIER ASSIGN expression SEMICOLON {
    char* type = $1;
    if (isAssignmentCompatible(type, $4->type)) {
        addSymbol($2, type, currentScope, 0);
        $$ = createNode("Assignment", type, createNode($2, type, NULL, NULL), $4);
        $$->value = $4->value;
        symbolTable[lookupSymbol($2, currentScope)].value = $4->value;
    } else {
        $$ = createNode("Error", "error", NULL, NULL);
    }
}
```

### 4. Function Overloading

```
Rule: function_declaration → type identifier ( parameter_list ) { statements }
Action: {
    check_unique_function_signature(identifier, type, parameter_types);
    add_function_to_table(identifier, type, parameters);
    enter_function_scope();
    add_parameters_to_symbol_table();
    process_function_body();
    exit_function_scope();
}
```

The parser implements this through the `addFunction` and `lookupFunctionOverload` functions.

## Conclusion

Our parser implements an L-attributed grammar that uses both synthesized and inherited attributes. The semantic actions are embedded in the grammar rules using the standard YACC/Bison approach, with appropriate evaluations that respect the dependencies between attributes. This enables the parser to handle complex features like type checking, scope management, and function overloading while properly evaluating expression values.
