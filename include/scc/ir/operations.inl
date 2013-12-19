// Mnemonic, Code, Inputs, Returns (0 or 1), Description

OP(nop,         NOP,              0, 0, "Do nothing.")

//
// Memory
//

OP(load,        LOAD,             2, 1, "Loads a value through a pointer.")
OP(store,       STORE,            2, 0, "Stores a value through a pointer.")

OP(phi,         PHI,              2, 1, "Chooses a value based on path taken.")

OP(swizzle,     SWIZZLE,          2, 1, "Swizzles input by mask.")

//
// Storage
//

OP(fetch,       FETCH,            2, 1, "Samples a texel.")
OP(gather,      GATHER,           3, 1, "Samples a 2x2 block of texels.")

//
// Arithmetic
//

OP(add,         ADD,              2, 1, "Adds second input to first input.")
OP(sub,         SUB,              1, 1, "Subtracts second input from first input.")
OP(mul,         MULTIPLY,         1, 1, "Multiplies first input with second input.")
OP(div,         DIVIDE,           1, 1, "Divides first input by second input.")

OP(fma,         FMA,              3, 1, "Multiplies first input by second input and adds third input, then rounds.")

OP(sqrt,        SQRT,             1, 1, "Computes square root of value.")
OP(rsqrt,       RSQRT,            1, 1, "Computes reciprocal square root of value.")

//
// Trigonometry
//

OP(sin,         SIN,              1, 1, "Sine of input.")
OP(cos,         COS,              1, 1, "Cosine of input.")
OP(tan,         TAN,              1, 1, "Tangent of input.")

OP(sinh,        SINH,             1, 1, "Hyperbolic sine of input.")
OP(cosh,        COSH,             1, 1, "Hyperbolic cosine of input.")
OP(tanh,        TANH,             1, 1, "Hyperbolic tangent of input.")

OP(asin,        ASIN,             1, 1, "Arc sine of input.")
OP(acos,        ACOS,             1, 1, "Arc cosine of input.")
OP(atan,        ATAN,             1, 1, "Arc tangent of input.")
OP(atan2,       ATAN2,            2, 1, "Arc tangent of inputs for all quadrants.")

//
// Exponentiation and Logarithms
//

OP(pow,         POW,              2, 1, "Raises first input to the power of the second input.")

OP(exp,         EXP,              1, 1, "Computes natural exponentiation of input.")
OP(exp2,        EXP2,             1, 1, "Computes 2 raised to the power of input.")
OP(exp10,       EXP10,            1, 1, "Computes 10 raised to the power of input.")

OP(log,         LOG,              2, 1, "Computes logarithm base first input of second input.")
OP(log2,        LOG2,             1, 1, "Computes natural logarithm of input.")
OP(log10,       LOG10,            1, 1, "Computes logarithm base-10 of input.")

//
// Vectors
//

OP(magnitude,   MAGNITUDE,        1, 1, "Computes magnitude of input vector.")
OP(length,      LENGTH,           1, 1, "Alias for `magnitude`.")

OP(dot,         DOT,              2, 1, "Computes dot product of input vectors.")
OP(cross,       CROSS,            2, 1, "Computes cross product of first vector by second vector.")

OP(normalize,   NORMALIZE,        1, 1, "Normalizes input vector.")

OP(distance,    DISTANCE,         2, 1, "Computes distance from first vector to second vector.")

OP(reflect,     REFLECT,          2, 1, "Computes incident ray reflected against normal.")
OP(refract,     REFRACT,          3, 1, "Computes incident ray refracted against normal and eta.")

//
// Matricies
//

OP(transpose,   TRANSPOSE,        1, 1, "Transposes matrix.")
OP(inverse,     INVERSE,          1, 1, "Computes inverse of matrix.")
OP(determinant, DETERMINANT,      1, 1, "Computes determinant of matrix.")

//
// Intrinsics
//

OP(abs,         ABS,              1, 1, "Returns component-wise absolute of value.")

OP(floor,       FLOOR,            1, 1, "Returns component-wise floor of value.")
OP(ceil,        CEIL,             1, 1, "Returns component-wise ceiling of value.")

OP(min,         MIN,              2, 1, "Returns component-wise lesser of inputs.")
OP(max,         MAX,              2, 1, "Returns component-wise greater of inputs.")

OP(clamp,       CLAMP,            3, 1, "Component-wise constrains value between minimum and maximum values.")

//
// Comparisions
//

OP(lt,          LESS,             2, 1, "Returns non-zero value if first input is less than second input.")
OP(lte,         LESS_OR_EQUAL,    2, 1, "Returns non-zero value if first input is less than or equal to second input.")
OP(eq,          EQUAL,            2, 1, "Returns non-zero value if first input is equal to second input.")
OP(neq,         NOT_EQUAL,        2, 1, "Returns non-zero value if first input is not equal to second input.")
OP(gt,          GREATER,          2, 1, "Returns non-zero value if first input is greater than second input.")
OP(gte,         GREATER_OR_EQUAL, 2, 1, "Returns non-zero value if first input is greater than or equal to second input.")

//
// Control Flow
//

// TODO(mtwilliams): Call and return instructions?

OP(jmp,         JUMP,             1, 0, "Jumps to label.")
OP(branch,      BRANCH,           2, 0, "Jump to label if input is not zero.")

//
// Special
//

OP(discard,     DISCARD,          0, 0, "Flag results of program to be discarded.")

#if 0

frac
trunc

all // All above 0
any // Any above 0

neg // Negates

not
and
or
xor

step

lerp/mix
slerp

#endif
