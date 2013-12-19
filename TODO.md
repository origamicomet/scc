### Immediate

1. Document `scc/lexer.h` and `scc/feed.h`.
2. Finish intermediate representation lexer.
3. Parse intermediate representation.
 4. Generate intermediate representation using a builder.
5. Implement a crap tonne of sanity checks in builder.
6. Document builder.
7. Setup testing infrastructure.
 * Build a relatively large test suite to cover IR.
  * Then (re)write `vigilant` and use it to manage test suites.
   * C/C++, not Ruby!
   * Seperate process to protect against faults.
8. Publish.
9. Improve build infrastructure.
 * AppVeyor and Travis builds.
  * Refactor Ryb a lot.

### Nebulous

TODO:
  * Generate DOT files (for Graphviz).
    * Layout engines are terrible; write my own?
  * Optimize compilation by making "modules" first class?
  * "Asynchronous" pipeline a la Jai.
  * Unicode
    * Implement UTF-8 encoding and decoding.
    * Implement Unicode canonicalization.
    * Fix assumptions about ASCII.
  * Optimizations
    * Strength reduction.
    * Instruction selection pass.
  * Analysis
    * Poisoning.
      * Yell at user for undefined behavior. 

PERF:
  * File/feed caching to improve pre-processor `#include` performance.
    * Ideally, we would compile all shaders with one invocation of the driver.
      * The driver could take hints about "ONE" or "MULTIPLE." If more than one, but hinted otherwise, yell at the user and then setup caching.

REFACTOR:
  * Replace `SCC_TRUE` and `SCC_FALSE` with `true` and `false`?
  * Refactor number parsing in `scc_ir_lexer_handle_number` into two shared functions...
     * One that sanity checks the number and determines its components.
        * Indicate if a binary, octal, decimal, or hexadecimal number by checking significand.
        * Determine width.
        * Extract sign.
        * Extract exponent, if specified.
        * Indicate relevant spans...
           * Sign
           * Significand
           * Integer part
           * Fractional part
           * Exponent
     * Another that parses it into a fitting type.
        * A signed or unsigned 64-bit integer.
        * A signed 64-bit floating-point number.
        * Check for `ERANGE` when parsing.
  * Handle old control characters.
  * Only accept UTF-8 in comments.
  * Generate and use a radix tree to make matching faster.
    * Finite stage recognizers are cool, but overkill. Future work?

### Custom graph visualization tool.

https://en.wikipedia.org/wiki/Graph_drawing

 Shader Cross-Compiler
 ---

  Translator
   Maps intermediate representation to graph.
    Reflects modifications?

 Tool
 ---
  
  Parser
   Constitutes graph from a language like DOT.

  Builder
   Aids in building a graph representation.
    Tries to avoid pointer chasing; indexes rather than pointers.

  Selector/Selection
   Refers to a portion of the graph.
    In our case, perhaps a function and optionally dependencies.
   Visual selection may differ from logical selection, e.g. "zoom."

  LayoutEngine
   Lays out graph, ideally planar.

  Library
   Various graph components.
    Components may be defined in a language like DOT.

  Presenter
   Caching generates flat list, a mapping from source to flattened span.
    Makes drawing fast!
   Presents a graph on canvas.
    Can rely on transparency to allow layers!

  Canvas
   Immediate mode rendering of two-dimensional primitives, like rectangles and lines.
    Save to image, offline.
    Draw in real-time through OpenGL.
