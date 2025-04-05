v=Starting parse
Entering state 0
Stack now 0
Reading a token
Next token is token INT ()
Shifting token INT ()
Entering state 7
Stack now 0 7
Reducing stack by rule 98 (line 378):
   $1 = token INT ()
-> $$ = nterm type_specifier ()
Entering state 30
Stack now 0 30
Reducing stack by rule 136 (line 466):
   $1 = nterm type_specifier ()
-> $$ = nterm specifier_qualifier_list ()
Entering state 34
Stack now 0 34
Reading a token
Next token is token IDENTIFIER ()
Reducing stack by rule 84 (line 348):
   $1 = nterm specifier_qualifier_list ()
-> $$ = nterm declaration_specifiers ()
Entering state 28
Stack now 0 28
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 49
Stack now 0 28 49
Reducing stack by rule 156 (line 518):
   $1 = token IDENTIFIER ()
-> $$ = nterm direct_declarator ()
Entering state 56
Stack now 0 28 56
Reading a token
Next token is token LEFT_PAREN ()
Shifting token LEFT_PAREN ()
Entering state 81
Stack now 0 28 56 81
Reading a token
Next token is token RIGHT_PAREN ()
Shifting token RIGHT_PAREN ()
Entering state 137
Stack now 0 28 56 81 137
Reducing stack by rule 160 (line 523):
   $1 = nterm direct_declarator ()
   $2 = token LEFT_PAREN ()
   $3 = token RIGHT_PAREN ()
-> $$ = nterm direct_declarator ()
Entering state 56
Stack now 0 28 56
Reading a token
Next token is token LEFT_CURLY ()
Reducing stack by rule 155 (line 513):
   $1 = nterm direct_declarator ()
-> $$ = nterm declarator ()
Entering state 55
Stack now 0 28 55
Next token is token LEFT_CURLY ()
Reducing stack by rule 234 (line 711):
-> $$ = nterm $@7 ()
Entering state 80
Stack now 0 28 55 80
Next token is token LEFT_CURLY ()
Shifting token LEFT_CURLY ()
Entering state 135
Stack now 0 28 55 80 135
Reading a token
Next token is token INT ()
Reducing stack by rule 201 (line 631):
-> $$ = nterm $@5 ()
Entering state 203
Stack now 0 28 55 80 135 203
Next token is token INT ()
Shifting token INT ()
Entering state 7
Stack now 0 28 55 80 135 203 7
Reducing stack by rule 98 (line 378):
   $1 = token INT ()
-> $$ = nterm type_specifier ()
Entering state 30
Stack now 0 28 55 80 135 203 30
Reducing stack by rule 136 (line 466):
   $1 = nterm type_specifier ()
-> $$ = nterm specifier_qualifier_list ()
Entering state 34
Stack now 0 28 55 80 135 203 34
Reading a token
Next token is token IDENTIFIER ()
Reducing stack by rule 84 (line 348):
   $1 = nterm specifier_qualifier_list ()
-> $$ = nterm declaration_specifiers ()
Entering state 276
Stack now 0 28 55 80 135 203 276
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 49
Stack now 0 28 55 80 135 203 276 49
Reducing stack by rule 156 (line 518):
   $1 = token IDENTIFIER ()
-> $$ = nterm direct_declarator ()
Entering state 56
Stack now 0 28 55 80 135 203 276 56
Reading a token
Next token is token ASSIGN ()
Reducing stack by rule 155 (line 513):
   $1 = nterm direct_declarator ()
-> $$ = nterm declarator ()
Entering state 101
Stack now 0 28 55 80 135 203 276 101
Next token is token ASSIGN ()
Shifting token ASSIGN ()
Entering state 79
Stack now 0 28 55 80 135 203 276 101 79
Reading a token
Next token is token I_CONSTANT ()
Shifting token I_CONSTANT ()
Entering state 110
Stack now 0 28 55 80 135 203 276 101 79 110
Reducing stack by rule 6 (line 186):
   $1 = token I_CONSTANT ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 276 101 79 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 276 101 79 118
Reading a token
Next token is token SEMICOLON ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 276 101 79 119
Next token is token SEMICOLON ()
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 276 101 79 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 276 101 79 122
Next token is token SEMICOLON ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 276 101 79 123
Next token is token SEMICOLON ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 276 101 79 124
Next token is token SEMICOLON ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 276 101 79 125
Next token is token SEMICOLON ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 276 101 79 126
Next token is token SEMICOLON ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 276 101 79 127
Next token is token SEMICOLON ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 276 101 79 128
Next token is token SEMICOLON ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 276 101 79 129
Next token is token SEMICOLON ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 276 101 79 130
Next token is token SEMICOLON ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 276 101 79 131
Next token is token SEMICOLON ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 132
Stack now 0 28 55 80 135 203 276 101 79 132
Reducing stack by rule 65 (line 313):
   $1 = nterm conditional_expression ()
-> $$ = nterm assignment_expression ()
Entering state 133
Stack now 0 28 55 80 135 203 276 101 79 133
Reducing stack by rule 188 (line 593):
   $1 = nterm assignment_expression ()
-> $$ = nterm initializer ()
Entering state 134
Stack now 0 28 55 80 135 203 276 101 79 134
Reducing stack by rule 89 (line 361):
   $1 = nterm declarator ()
   $2 = token ASSIGN ()
   $3 = nterm initializer ()
-> $$ = nterm init_declarator ()
Entering state 53
Stack now 0 28 55 80 135 203 276 53
Reducing stack by rule 86 (line 354):
   $1 = nterm init_declarator ()
-> $$ = nterm init_declarator_list ()
Entering state 52
Stack now 0 28 55 80 135 203 276 52
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 77
Stack now 0 28 55 80 135 203 276 52 77
Reducing stack by rule 81 (line 341):
   $1 = nterm declaration_specifiers ()
   $2 = nterm init_declarator_list ()
   $3 = token SEMICOLON ()
checking sizes0 0
inside checking variadic function 1
-> $$ = nterm declaration ()
Entering state 275
Stack now 0 28 55 80 135 203 275
Reducing stack by rule 207 (line 651):
   $1 = nterm declaration ()
-> $$ = nterm declaration_list ()
Entering state 281
Stack now 0 28 55 80 135 203 281
Reading a token
Next token is token INT ()
Shifting token INT ()
Entering state 7
Stack now 0 28 55 80 135 203 281 7
Reducing stack by rule 98 (line 378):
   $1 = token INT ()
-> $$ = nterm type_specifier ()
Entering state 30
Stack now 0 28 55 80 135 203 281 30
Reducing stack by rule 136 (line 466):
   $1 = nterm type_specifier ()
-> $$ = nterm specifier_qualifier_list ()
Entering state 34
Stack now 0 28 55 80 135 203 281 34
Reading a token
Next token is token IDENTIFIER ()
Reducing stack by rule 84 (line 348):
   $1 = nterm specifier_qualifier_list ()
-> $$ = nterm declaration_specifiers ()
Entering state 276
Stack now 0 28 55 80 135 203 281 276
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 49
Stack now 0 28 55 80 135 203 281 276 49
Reducing stack by rule 156 (line 518):
   $1 = token IDENTIFIER ()
-> $$ = nterm direct_declarator ()
Entering state 56
Stack now 0 28 55 80 135 203 281 276 56
Reading a token
Next token is token SEMICOLON ()
Reducing stack by rule 155 (line 513):
   $1 = nterm direct_declarator ()
-> $$ = nterm declarator ()
Entering state 101
Stack now 0 28 55 80 135 203 281 276 101
Next token is token SEMICOLON ()
Reducing stack by rule 88 (line 360):
   $1 = nterm declarator ()
-> $$ = nterm init_declarator ()
Entering state 53
Stack now 0 28 55 80 135 203 281 276 53
Reducing stack by rule 86 (line 354):
   $1 = nterm init_declarator ()
-> $$ = nterm init_declarator_list ()
Entering state 52
Stack now 0 28 55 80 135 203 281 276 52
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 77
Stack now 0 28 55 80 135 203 281 276 52 77
Reducing stack by rule 81 (line 341):
   $1 = nterm declaration_specifiers ()
   $2 = nterm init_declarator_list ()
   $3 = token SEMICOLON ()
checking sizes0 0
inside checking variadic function 1
-> $$ = nterm declaration ()
Entering state 333
Stack now 0 28 55 80 135 203 281 333
Reducing stack by rule 208 (line 652):
   $1 = nterm declaration_list ()
   $2 = nterm declaration ()
-> $$ = nterm declaration_list ()
Entering state 281
Stack now 0 28 55 80 135 203 281
Reading a token
Next token is token SWITCH ()
Reducing stack by rule 203 (line 643):
   $1 = nterm declaration_list ()
-> $$ = nterm declaration_statement_list ()
Entering state 280
Stack now 0 28 55 80 135 203 280
Next token is token SWITCH ()
Shifting token SWITCH ()
Entering state 269
Stack now 0 28 55 80 135 203 280 269
Reading a token
Next token is token LEFT_PAREN ()
Shifting token LEFT_PAREN ()
Entering state 325
Stack now 0 28 55 80 135 203 280 269 325
Reading a token
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 109
Stack now 0 28 55 80 135 203 280 269 325 109
Reducing stack by rule 5 (line 185):
   $1 = token IDENTIFIER ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 118
Reading a token
Next token is token RIGHT_PAREN ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 280 269 325 119
Next token is token RIGHT_PAREN ()
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 280 269 325 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 280 269 325 122
Next token is token RIGHT_PAREN ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 280 269 325 123
Next token is token RIGHT_PAREN ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 280 269 325 124
Next token is token RIGHT_PAREN ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 280 269 325 125
Next token is token RIGHT_PAREN ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 280 269 325 126
Next token is token RIGHT_PAREN ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 280 269 325 127
Next token is token RIGHT_PAREN ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 280 269 325 128
Next token is token RIGHT_PAREN ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 280 269 325 129
Next token is token RIGHT_PAREN ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 280 269 325 130
Next token is token RIGHT_PAREN ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 280 269 325 131
Next token is token RIGHT_PAREN ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 132
Stack now 0 28 55 80 135 203 280 269 325 132
Reducing stack by rule 65 (line 313):
   $1 = nterm conditional_expression ()
-> $$ = nterm assignment_expression ()
Entering state 160
Stack now 0 28 55 80 135 203 280 269 325 160
Reducing stack by rule 78 (line 334):
   $1 = nterm assignment_expression ()
-> $$ = nterm expression ()
Entering state 360
Stack now 0 28 55 80 135 203 280 269 325 360
Next token is token RIGHT_PAREN ()
Shifting token RIGHT_PAREN ()
Entering state 374
Stack now 0 28 55 80 135 203 280 269 325 360 374
Reading a token
Next token is token LEFT_CURLY ()
Shifting token LEFT_CURLY ()
Entering state 135
Stack now 0 28 55 80 135 203 280 269 325 360 374 135
Reading a token
Next token is token CASE ()
Reducing stack by rule 201 (line 631):
-> $$ = nterm $@5 ()
Entering state 203
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203
Next token is token CASE ()
Shifting token CASE ()
Entering state 264
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264
Reading a token
Next token is token I_CONSTANT ()
Shifting token I_CONSTANT ()
Entering state 110
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 110
Reducing stack by rule 6 (line 186):
   $1 = token I_CONSTANT ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 118
Reading a token
Next token is token COLON ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 142
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 142
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 122
Next token is token COLON ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 123
Next token is token COLON ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 124
Next token is token COLON ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 125
Next token is token COLON ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 126
Next token is token COLON ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 127
Next token is token COLON ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 128
Next token is token COLON ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 129
Next token is token COLON ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 130
Next token is token COLON ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 131
Next token is token COLON ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 319
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319
Next token is token COLON ()
Shifting token COLON ()
Entering state 353
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353
Reading a token
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 258
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 258
Reading a token
Next token is token ASSIGN ()
Reducing stack by rule 5 (line 185):
   $1 = token IDENTIFIER ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 118
Next token is token ASSIGN ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119
Next token is token ASSIGN ()
Shifting token ASSIGN ()
Entering state 180
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 180
Reducing stack by rule 67 (line 319):
   $1 = token ASSIGN ()
-> $$ = nterm assignment_operator ()
Entering state 181
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181
Reading a token
Next token is token I_CONSTANT ()
Shifting token I_CONSTANT ()
Entering state 110
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 110
Reducing stack by rule 6 (line 186):
   $1 = token I_CONSTANT ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 118
Reading a token
Next token is token SEMICOLON ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 119
Next token is token SEMICOLON ()
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 122
Next token is token SEMICOLON ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 123
Next token is token SEMICOLON ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 124
Next token is token SEMICOLON ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 125
Next token is token SEMICOLON ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 126
Next token is token SEMICOLON ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 127
Next token is token SEMICOLON ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 128
Next token is token SEMICOLON ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 129
Next token is token SEMICOLON ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 130
Next token is token SEMICOLON ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 131
Next token is token SEMICOLON ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 132
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 132
Reducing stack by rule 65 (line 313):
   $1 = nterm conditional_expression ()
-> $$ = nterm assignment_expression ()
Entering state 237
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 119 181 237
Reducing stack by rule 66 (line 314):
   $1 = nterm unary_expression ()
   $2 = nterm assignment_operator ()
   $3 = nterm assignment_expression ()
-> $$ = nterm assignment_expression ()
Entering state 160
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 160
Reducing stack by rule 78 (line 334):
   $1 = nterm assignment_expression ()
-> $$ = nterm expression ()
Entering state 274
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 274
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 329
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 274 329
Reducing stack by rule 212 (line 664):
   $1 = nterm expression ()
   $2 = token SEMICOLON ()
-> $$ = nterm expression_statement ()
Entering state 283
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 283
Reducing stack by rule 191 (line 607):
   $1 = nterm expression_statement ()
-> $$ = nterm statement ()
Entering state 369
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 264 319 353 369
Reducing stack by rule 197 (line 617):
   $1 = token CASE ()
   $2 = nterm conditional_expression ()
   $3 = token COLON ()
   $4 = nterm statement ()
-> $$ = nterm labeled_statement ()
Entering state 278
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 278
Reducing stack by rule 189 (line 605):
   $1 = nterm labeled_statement ()
-> $$ = nterm statement ()
Entering state 277
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 277
Reducing stack by rule 209 (line 657):
   $1 = nterm statement ()
-> $$ = nterm statement_list ()
Entering state 282
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282
Reading a token
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 258
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 258
Reading a token
Next token is token ASSIGN ()
Reducing stack by rule 5 (line 185):
   $1 = token IDENTIFIER ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 118
Next token is token ASSIGN ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119
Next token is token ASSIGN ()
Shifting token ASSIGN ()
Entering state 180
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 180
Reducing stack by rule 67 (line 319):
   $1 = token ASSIGN ()
-> $$ = nterm assignment_operator ()
Entering state 181
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181
Reading a token
Next token is token I_CONSTANT ()
Shifting token I_CONSTANT ()
Entering state 110
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 110
Reducing stack by rule 6 (line 186):
   $1 = token I_CONSTANT ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 118
Reading a token
Next token is token SEMICOLON ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 119
Next token is token SEMICOLON ()
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 122
Next token is token SEMICOLON ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 123
Next token is token SEMICOLON ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 124
Next token is token SEMICOLON ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 125
Next token is token SEMICOLON ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 126
Next token is token SEMICOLON ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 127
Next token is token SEMICOLON ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 128
Next token is token SEMICOLON ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 129
Next token is token SEMICOLON ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 130
Next token is token SEMICOLON ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 131
Next token is token SEMICOLON ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 132
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 132
Reducing stack by rule 65 (line 313):
   $1 = nterm conditional_expression ()
-> $$ = nterm assignment_expression ()
Entering state 237
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 119 181 237
Reducing stack by rule 66 (line 314):
   $1 = nterm unary_expression ()
   $2 = nterm assignment_operator ()
   $3 = nterm assignment_expression ()
-> $$ = nterm assignment_expression ()
Entering state 160
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 160
Reducing stack by rule 78 (line 334):
   $1 = nterm assignment_expression ()
-> $$ = nterm expression ()
Entering state 274
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 274
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 329
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 274 329
Reducing stack by rule 212 (line 664):
   $1 = nterm expression ()
   $2 = token SEMICOLON ()
-> $$ = nterm expression_statement ()
Entering state 283
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 283
Reducing stack by rule 191 (line 607):
   $1 = nterm expression_statement ()
-> $$ = nterm statement ()
Entering state 334
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 334
Reducing stack by rule 210 (line 658):
   $1 = nterm statement_list ()
   $2 = nterm statement ()
-> $$ = nterm statement_list ()
Entering state 282
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282
Reading a token
Next token is token BREAK ()
Shifting token BREAK ()
Entering state 262
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 262
Reading a token
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 316
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 262 316
Reducing stack by rule 227 (line 692):
   $1 = token BREAK ()
   $2 = token SEMICOLON ()
-> $$ = nterm jump_statement ()
Entering state 287
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 287
Reducing stack by rule 194 (line 610):
   $1 = nterm jump_statement ()
-> $$ = nterm statement ()
Entering state 334
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 334
Reducing stack by rule 210 (line 658):
   $1 = nterm statement_list ()
   $2 = nterm statement ()
-> $$ = nterm statement_list ()
Entering state 282
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282
Reading a token
Next token is token CASE ()
Shifting token CASE ()
Entering state 264
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264
Reading a token
Next token is token I_CONSTANT ()
Shifting token I_CONSTANT ()
Entering state 110
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 110
Reducing stack by rule 6 (line 186):
   $1 = token I_CONSTANT ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 118
Reading a token
Next token is token COLON ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 142
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 142
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 122
Next token is token COLON ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 123
Next token is token COLON ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 124
Next token is token COLON ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 125
Next token is token COLON ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 126
Next token is token COLON ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 127
Next token is token COLON ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 128
Next token is token COLON ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 129
Next token is token COLON ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 130
Next token is token COLON ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 131
Next token is token COLON ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 319
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319
Next token is token COLON ()
Shifting token COLON ()
Entering state 353
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353
Reading a token
Next token is token LEFT_CURLY ()
Shifting token LEFT_CURLY ()
Entering state 135
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135
Reading a token
Next token is token INT ()
Reducing stack by rule 201 (line 631):
-> $$ = nterm $@5 ()
Entering state 203
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203
Next token is token INT ()
Shifting token INT ()
Entering state 7
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 7
Reducing stack by rule 98 (line 378):
   $1 = token INT ()
-> $$ = nterm type_specifier ()
Entering state 30
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 30
Reducing stack by rule 136 (line 466):
   $1 = nterm type_specifier ()
-> $$ = nterm specifier_qualifier_list ()
Entering state 34
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 34
Reading a token
Next token is token IDENTIFIER ()
Reducing stack by rule 84 (line 348):
   $1 = nterm specifier_qualifier_list ()
-> $$ = nterm declaration_specifiers ()
Entering state 276
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276
Next token is token IDENTIFIER ()
Shifting token IDENTIFIER ()
Entering state 49
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 49
Reducing stack by rule 156 (line 518):
   $1 = token IDENTIFIER ()
-> $$ = nterm direct_declarator ()
Entering state 56
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 56
Reading a token
Next token is token ASSIGN ()
Reducing stack by rule 155 (line 513):
   $1 = nterm direct_declarator ()
-> $$ = nterm declarator ()
Entering state 101
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101
Next token is token ASSIGN ()
Shifting token ASSIGN ()
Entering state 79
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79
Reading a token
Next token is token I_CONSTANT ()
Shifting token I_CONSTANT ()
Entering state 110
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 110
Reducing stack by rule 6 (line 186):
   $1 = token I_CONSTANT ()
-> $$ = nterm primary_expression ()
Entering state 117
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 117
Reducing stack by rule 13 (line 201):
   $1 = nterm primary_expression ()
-> $$ = nterm postfix_expression ()
Entering state 118
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 118
Reading a token
Next token is token SEMICOLON ()
Reducing stack by rule 21 (line 213):
   $1 = nterm postfix_expression ()
-> $$ = nterm unary_expression ()
Entering state 119
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 119
Next token is token SEMICOLON ()
Reducing stack by rule 33 (line 233):
   $1 = nterm unary_expression ()
-> $$ = nterm cast_expression ()
Entering state 121
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 121
Reducing stack by rule 35 (line 239):
   $1 = nterm cast_expression ()
-> $$ = nterm multiplicative_expression ()
Entering state 122
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 122
Next token is token SEMICOLON ()
Reducing stack by rule 39 (line 247):
   $1 = nterm multiplicative_expression ()
-> $$ = nterm additive_expression ()
Entering state 123
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 123
Next token is token SEMICOLON ()
Reducing stack by rule 42 (line 254):
   $1 = nterm additive_expression ()
-> $$ = nterm shift_expression ()
Entering state 124
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 124
Next token is token SEMICOLON ()
Reducing stack by rule 45 (line 261):
   $1 = nterm shift_expression ()
-> $$ = nterm relational_expression ()
Entering state 125
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 125
Next token is token SEMICOLON ()
Reducing stack by rule 50 (line 270):
   $1 = nterm relational_expression ()
-> $$ = nterm equality_expression ()
Entering state 126
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 126
Next token is token SEMICOLON ()
Reducing stack by rule 53 (line 277):
   $1 = nterm equality_expression ()
-> $$ = nterm and_expression ()
Entering state 127
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 127
Next token is token SEMICOLON ()
Reducing stack by rule 55 (line 283):
   $1 = nterm and_expression ()
-> $$ = nterm xor_expression ()
Entering state 128
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 128
Next token is token SEMICOLON ()
Reducing stack by rule 57 (line 289):
   $1 = nterm xor_expression ()
-> $$ = nterm or_expression ()
Entering state 129
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 129
Next token is token SEMICOLON ()
Reducing stack by rule 59 (line 295):
   $1 = nterm or_expression ()
-> $$ = nterm logical_and_expression ()
Entering state 130
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 130
Next token is token SEMICOLON ()
Reducing stack by rule 61 (line 301):
   $1 = nterm logical_and_expression ()
-> $$ = nterm logical_or_expression ()
Entering state 131
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 131
Next token is token SEMICOLON ()
Reducing stack by rule 63 (line 307):
   $1 = nterm logical_or_expression ()
-> $$ = nterm conditional_expression ()
Entering state 132
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 132
Reducing stack by rule 65 (line 313):
   $1 = nterm conditional_expression ()
-> $$ = nterm assignment_expression ()
Entering state 133
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 133
Reducing stack by rule 188 (line 593):
   $1 = nterm assignment_expression ()
-> $$ = nterm initializer ()
Entering state 134
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 101 79 134
Reducing stack by rule 89 (line 361):
   $1 = nterm declarator ()
   $2 = token ASSIGN ()
   $3 = nterm initializer ()
-> $$ = nterm init_declarator ()
Entering state 53
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 53
Reducing stack by rule 86 (line 354):
   $1 = nterm init_declarator ()
-> $$ = nterm init_declarator_list ()
Entering state 52
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 52
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 77
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 276 52 77
Reducing stack by rule 81 (line 341):
   $1 = nterm declaration_specifiers ()
   $2 = nterm init_declarator_list ()
   $3 = token SEMICOLON ()
checking sizes0 0
inside checking variadic function 1
-> $$ = nterm declaration ()
Entering state 275
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 275
Reducing stack by rule 207 (line 651):
   $1 = nterm declaration ()
-> $$ = nterm declaration_list ()
Entering state 281
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 281
Reading a token
Next token is token BREAK ()
Reducing stack by rule 203 (line 643):
   $1 = nterm declaration_list ()
-> $$ = nterm declaration_statement_list ()
Entering state 280
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280
Next token is token BREAK ()
Shifting token BREAK ()
Entering state 262
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280 262
Reading a token
Next token is token SEMICOLON ()
Shifting token SEMICOLON ()
Entering state 316
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280 262 316
Reducing stack by rule 227 (line 692):
   $1 = token BREAK ()
   $2 = token SEMICOLON ()
-> $$ = nterm jump_statement ()
Entering state 287
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280 287
Reducing stack by rule 194 (line 610):
   $1 = nterm jump_statement ()
-> $$ = nterm statement ()
Entering state 277
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280 277
Reducing stack by rule 209 (line 657):
   $1 = nterm statement ()
-> $$ = nterm statement_list ()
Entering state 332
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280 332
Reading a token
Next token is token RIGHT_CURLY ()
Reducing stack by rule 206 (line 646):
   $1 = nterm declaration_statement_list ()
   $2 = nterm statement_list ()
-> $$ = nterm declaration_statement_list ()
Entering state 280
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280
Next token is token RIGHT_CURLY ()
Shifting token RIGHT_CURLY ()
Entering state 330
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 135 203 280 330
Reducing stack by rule 202 (line 631):
   $1 = token LEFT_CURLY ()
   $2 = nterm $@5 ()
   $3 = nterm declaration_statement_list ()
   $4 = token RIGHT_CURLY ()
-> $$ = nterm compound_statement ()
Entering state 279
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 279
Reducing stack by rule 190 (line 606):
   $1 = nterm compound_statement ()
-> $$ = nterm statement ()
Entering state 369
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 264 319 353 369
Reducing stack by rule 197 (line 617):
   $1 = token CASE ()
   $2 = nterm conditional_expression ()
   $3 = token COLON ()
   $4 = nterm statement ()
-> $$ = nterm labeled_statement ()
Entering state 278
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 278
Reducing stack by rule 189 (line 605):
   $1 = nterm labeled_statement ()
-> $$ = nterm statement ()
Entering state 334
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282 334
Reducing stack by rule 210 (line 658):
   $1 = nterm statement_list ()
   $2 = nterm statement ()
-> $$ = nterm statement_list ()
Entering state 282
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 282
Reading a token
Next token is token RIGHT_CURLY ()
Reducing stack by rule 204 (line 644):
   $1 = nterm statement_list ()
-> $$ = nterm declaration_statement_list ()
Entering state 280
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 280
Next token is token RIGHT_CURLY ()
Shifting token RIGHT_CURLY ()
Entering state 330
Stack now 0 28 55 80 135 203 280 269 325 360 374 135 203 280 330
Reducing stack by rule 202 (line 631):
   $1 = token LEFT_CURLY ()
   $2 = nterm $@5 ()
   $3 = nterm declaration_statement_list ()
   $4 = token RIGHT_CURLY ()
-> $$ = nterm compound_statement ()
Entering state 279
Stack now 0 28 55 80 135 203 280 269 325 360 374 279
Reducing stack by rule 190 (line 606):
   $1 = nterm compound_statement ()
-> $$ = nterm statement ()
Entering state 383
Stack now 0 28 55 80 135 203 280 269 325 360 374 383
Reducing stack by rule 215 (line 671):
   $1 = token SWITCH ()
   $2 = token LEFT_PAREN ()
   $3 = nterm expression ()
   $4 = token RIGHT_PAREN ()
   $5 = nterm statement ()
-> $$ = nterm selection_statement ()
Entering state 284
Stack now 0 28 55 80 135 203 280 284
Reducing stack by rule 192 (line 608):
   $1 = nterm selection_statement ()
-> $$ = nterm statement ()
Entering state 277
Stack now 0 28 55 80 135 203 280 277
Reducing stack by rule 209 (line 657):
   $1 = nterm statement ()
-> $$ = nterm statement_list ()
Entering state 332
Stack now 0 28 55 80 135 203 280 332
Reading a token
Next token is token RIGHT_CURLY ()
Reducing stack by rule 206 (line 646):
   $1 = nterm declaration_statement_list ()
   $2 = nterm statement_list ()
-> $$ = nterm declaration_statement_list ()
Entering state 280
Stack now 0 28 55 80 135 203 280
Next token is token RIGHT_CURLY ()
Shifting token RIGHT_CURLY ()
Entering state 330
Stack now 0 28 55 80 135 203 280 330
Reducing stack by rule 202 (line 631):
   $1 = token LEFT_CURLY ()
   $2 = nterm $@5 ()
   $3 = nterm declaration_statement_list ()
   $4 = token RIGHT_CURLY ()
-> $$ = nterm compound_statement ()
Entering state 136
Stack now 0 28 55 80 136
Reducing stack by rule 235 (line 711):
   $1 = nterm declaration_specifiers ()
   $2 = nterm declarator ()
   $3 = nterm $@7 ()
   $4 = nterm compound_statement ()
-> $$ = nterm function_definition ()
Entering state 39
Stack now 0 39
Reducing stack by rule 232 (line 705):
   $1 = nterm function_definition ()
-> $$ = nterm external_declaration ()
Entering state 38
Stack now 0 38
Reducing stack by rule 230 (line 699):
   $1 = nterm external_declaration ()
-> $$ = nterm translation_unit ()
Entering state 37
Stack now 0 37
Reading a token
Now at end of input.
Shifting token "end of file" ()
Entering state 63
Stack now 0 37 63
Stack now 0 37 63
Cleanup: popping token "end of file" ()
Cleanup: popping nterm translation_unit ()
The Defined Types Table is empty.
The Typedef Table is empty.

SYMBOL TABLE:
----------------------------------------------------------------------------
| Name                | Type                      | Scope   | Offset       |
----------------------------------------------------------------------------
| main                | 5                         | 0       | 0            |
----------------------------------------------------------------------------
Parsing completed successfully.
