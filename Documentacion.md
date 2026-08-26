# Convenciones de Código — Compiladores 1

Guía de estilo y nomenclatura del proyecto. Todo el código nuevo debe seguir estas
reglas; el código existente se ajusta cuando se toque.

---

## 1. Nomenclatura general (C++)

| Elemento | Convención | Ejemplo |
| --- | --- | --- |
| Clase / Struct | `PascalCase` | `Lexer`, `SymbolTable`, `TokenStream` |
| Enum (tipo) | `PascalCase` | `TokenType`, `NodeKind` |
| Valor de enum (`enum class`) | `PascalCase` | `TokenType::Identifier` |
| Valor de enum (C clásico) | `MAYUSCULA_SNAKE` con prefijo | `TK_IDENTIFIER`, `TK_EOF` |
| Función / Método | `camelCase`, empieza con verbo | `nextToken()`, `parseExpression()` |
| Variable local | `camelCase` | `currentChar`, `lineNumber` |
| Parámetro | `camelCase` | `void advance(int steps)` |
| Atributo de clase (privado) | `camelCase` + sufijo `_` | `position_`, `sourceCode_` |
| Atributo de struct (público) | `camelCase` sin sufijo | `line`, `lexeme` |
| Constante / `constexpr` | `kPascalCase` | `kMaxIdentifierLength` |
| Macro / `#define` | `MAYUSCULA_SNAKE` | `MAX_BUFFER_SIZE` |
| Guardas de inclusión | `NOMBRE_ARCHIVO_HPP` | `TOKEN_HPP` |
| Namespace | `minusculas` (una palabra) | `namespace compilador { }` |
| Alias de tipo (`using`) | `PascalCase` | `using TokenList = std::vector<Token>;` |
| Plantilla (parámetro) | `PascalCase`, una letra o palabra | `template <typename T>` |

### Reglas transversales

- Nombres en **inglés**, sin acentos ni `ñ`; los comentarios pueden ir en español.
- Sin abreviaturas oscuras: `tokenCount`, no `tkCnt`. Se permiten las estándar:
  `id`, `num`, `str`, `ptr`, `idx`, `tmp`, `i/j/k` (solo en bucles).
- Booleanos con prefijo interrogativo: `isValid`, `hasNext`, `canRead`, `shouldStop`.
- Los getters no llevan `get` si devuelven un atributo directo: `line()`, no `getLine()`.
  Los setters sí: `setLine(int)`.

---

## 2. Archivos y carpetas

| Elemento | Convención | Ejemplo |
| --- | --- | --- |
| Cabecera | `PascalCase.hpp` (una clase por archivo) | `Token.hpp`, `Lexer.hpp` |
| Implementación | `PascalCase.cpp`, mismo nombre que su `.hpp` | `Lexer.cpp` |
| Punto de entrada | `main.cpp` | `main.cpp` |
| Carpeta de código | `minusculas` | `src/`, `include/`, `tests/` |
| Archivo de prueba | `PascalCaseTest.cpp` | `LexerTest.cpp` |
| Entrada de ejemplo | `snake_case.<ext>` | `hola_mundo.src` |
| Documentación | `PascalCase.md` | `Documentacion.md` |

---

## 3. Nomenclatura propia del compilador

| Componente | Convención | Ejemplo |
| --- | --- | --- |
| Fases | Sustantivo simple | `Lexer`, `Parser`, `SemanticAnalyzer`, `CodeGenerator` |
| Nodos del AST | Sufijo `Node` | `BinaryExprNode`, `IfStmtNode`, `FunctionDeclNode` |
| Tipos de token | Sustantivo en singular | `TokenType::Plus`, `TokenType::Identifier` |
| Errores | Sufijo `Error` | `LexicalError`, `SyntaxError`, `SemanticError` |
| Métodos de análisis | Prefijo `parse` + regla gramatical | `parseTerm()`, `parseStatement()` |
| Métodos de reconocimiento | Prefijo `match` / `expect` | `match(TokenType)`, `expect(TokenType)` |
| Recorridos del AST | Prefijo `visit` | `visitBinaryExpr()` |
| Símbolos de la tabla | Sufijo `Symbol` | `VariableSymbol`, `FunctionSymbol` |

---

## 4. Formato

| Regla | Valor |
| --- | --- |
| Indentación | 4 espacios, nunca tabs |
| Longitud de línea | máximo 100 caracteres |
| Llaves | misma línea (`K&R`) para funciones, clases y control |
| `else` / `catch` | en la misma línea que la llave de cierre: `} else {` |
| Bloques de una línea | siempre con llaves, aunque sea una sola sentencia |
| Espacios | `if (x)`, `a + b`, `f(a, b)` — sin espacio antes de `(` en llamadas |
| Punteros / referencias | tipo pegado al símbolo: `Token* tok`, `const Token& tok` |
| Líneas en blanco | 1 entre métodos, 2 entre secciones lógicas del archivo |
| Fin de archivo | siempre con salto de línea final |
| Orden en clase | `public:` → `protected:` → `private:` |

### Ejemplo

```cpp
class Lexer {
public:
    explicit Lexer(const std::string& source);

    Token nextToken();
    bool hasNext() const;

private:
    char peek() const;
    void skipWhitespace();

    std::string source_;
    size_t position_;
    int line_;
};
```

---

## 5. Orden de `#include`

Separados por una línea en blanco, en este orden:

```cpp
#include "Lexer.hpp"      // 1. cabecera propia del .cpp

#include <string>         // 2. biblioteca estándar
#include <vector>

#include "Token.hpp"      // 3. cabeceras del proyecto
#include "Error.hpp"
```

Cada grupo alfabético. `""` para el proyecto, `<>` para lo externo.

---

## 6. Comentarios

| Tipo | Formato | Uso |
| --- | --- | --- |
| Documentación | `///` o `/** */` antes de la declaración | Clases y métodos públicos |
| Explicativo | `//` con espacio después | Dentro de funciones |
| Sección | `// ---- Nombre ----` | Separar bloques largos |
| Pendiente | `// TODO(nombre): ...` | Trabajo por hacer |

Los comentarios explican **por qué**, no **qué**. Nada de código comentado en el
repositorio: eso lo guarda git.

```cpp
/// Devuelve el siguiente token sin consumirlo.
/// Retorna TokenType::EndOfFile si no queda entrada.
Token peekToken() const;
```

---

## 7. Mensajes de error del compilador

Formato único para toda salida al usuario:

```
archivo:linea:columna: <nivel>: <mensaje>
```

| Campo | Regla | Ejemplo |
| --- | --- | --- |
| Nivel | `error`, `warning`, `note` en minúscula | `error` |
| Mensaje | minúscula inicial, sin punto final | `caracter inesperado '@'` |
| Línea/columna | base 1 | `main.src:12:5` |

```
main.src:12:5: error: caracter inesperado '@'
main.src:14:1: warning: variable 'x' declarada pero no usada
```

---

## 8. Commits

`<tipo>: <descripción en imperativo>`

| Tipo | Uso |
| --- | --- |
| `feat` | funcionalidad nueva |
| `fix` | corrección de error |
| `refactor` | cambio interno sin alterar comportamiento |
| `docs` | documentación |
| `test` | pruebas |
| `chore` | build, configuración, limpieza |

```
feat: agregar reconocimiento de operadores relacionales al lexer
fix: corregir conteo de linea en comentarios multilinea
```
