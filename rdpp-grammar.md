## Syntactical Grammar Modifications

A context-free grammar is used to define the lexical and syntactical parts of the PLATYPUS language and the lexical and syntactic structure of a PLATYPUS program.

> **&lt;program&gt; -&gt; PROGRAM {&lt;opt_statements&gt;} <br>**
First Set:
FIRST(&lt;program&gt;) = {KW_T(PROGRAM)}

### Statements

- Optional Statements
> **&lt;opt_statements&gt; -&gt; &lt;statements&gt; | ϵ <br>**
> FIRST(&lt;opt_statements&gt;) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE),
KW_T(INPUT), KW_T(OUTPUT), ϵ }

- Statements

> Fixed Left Recursion <br>
> **&lt;statements&gt; -&gt; &lt;statements&gt; &lt;statement&gt; | &lt;statement&gt;** <br>

> **&lt;statements&gt; -&gt; &lt;statement&gt;&lt;statements’&gt;**<br>
> FIRST (&lt;statements&gt;) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT),
> KW_T(OUTPUT) }

> **&lt;statements’&gt; -&gt; &lt;statement&gt;&lt;statements’&gt; | ϵ** <br>
>FIRST (&lt;statements’&gt;) = { AVID_T, SVID_T, KW_T(IF), KW_T(WHILE), KW_T(INPUT),
> KW_T(OUTPUT), ϵ }

### TODO:

- Statement
- Assignment Statement
- Assignment Expression
- Selection Statement (if statement)
- Iteration Statement
- Input Statement
- Variable List
- Variable Identifier
- Optional Varible List

#### Expressions
- Arithmetic Expression + Unary
- Additive Arithmetic Expression
- Multiplicative Arithmetic Expression
- Primary Arithmetic Expression
- String Expression
- Primary String Expression
- Conditional Expression
- Logical OR Expression
- Logical AND Expression
- Logical NOT Expression
- Relational Expression
- Primary Arithmetic Relational Expression
