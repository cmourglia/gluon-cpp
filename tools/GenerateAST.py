import subprocess

expressions = {
    'Binary': ['Expr left', 'Token op', 'Expr right'],
    'Grouping': ['Expr expr'],
    'Literal': ['Value value'],
    'Unary': ['Token op', 'Expr right']
}

statements = {

}


def header():
    return '''\
#pragma once

#include <gluon/lang/token.h>
#include <gluon/lang/value.h>

#include <beard/core/macros.h>

#include <memory>

'''


def forward_declare(types, base_class='', prefix=''):
    decl = ''
    for type in types:
        decl += f'class {prefix}{type}{base_class};\n'
    decl += '\n'

    return decl


def visitor(base_class, types, prefix=''):
    visitor = f'class {base_class}Visitor\n'
    visitor += '{\n'
    visitor += 'public:\n'

    for type in types:
        visitor += f'\tvirtual {prefix}Value Visit{type}({prefix}{type}{base_class}& {type}) = 0;\n'
    visitor += '};\n\n'

    return visitor


def base_class(base_class, prefix=''):
    classname = f'{prefix}{base_class}'
    return f'''\
class {classname}
{{
public:
\tNONCOPYABLE({classname});
\tNONMOVEABLE({classname});
\t{classname}() = default;
\tvirtual ~{prefix}{base_class}() = default;

\tvirtual {prefix}Value Accept({base_class}Visitor& visitor) = 0;
}};

'''


def get_typename(type, prefix=''):
    typename = f'{prefix}{type}'
    if type != 'Value' and type != 'Token':
        typename = f'std::unique_ptr<{typename}>'
    return typename


def types(types, base_class='Expr', prefix=''):
    output = ''

    for expr in types:
        output += f'class {prefix}{expr}{base_class} : public {prefix}{base_class}\n'
        output += '{\n'
        output += 'public:\n'

        output += f'\texplicit {prefix}{expr}{base_class}('

        for i, field in enumerate(types[expr]):
            type, name = field.split(' ')
            type = get_typename(type, prefix)

            output += f'{type} {name}'
            if i != len(types[expr]) - 1:
                output += ', '

        output += ')\n'

        for i, field in enumerate(types[expr]):
            type, name = field.split(' ')
            type = get_typename(type, prefix)
            output += '\t\t'
            output += ':' if i == 0 else ','
            if type == 'Value':
                output += f' m_{name}{{{name}}}\n'
            else:
                output += f' m_{name}{{std::move({name})}}\n'

        output += '\t{\n'
        output += '\t}\n\n'

        output += f'\t{prefix}Value Accept({base_class}Visitor& Visitor) override\n'
        output += '\t{\n'
        output += f'\t\treturn Visitor.Visit{expr}(*this);\n'
        output += '\t}\n\n'

        for field in types[expr]:
            type, name = field.split(' ')
            if type == 'Value' or type == 'Token':
                output += f'\t{prefix}{type} {name}() const\n'
                output += '\t{\n'
                output += f'\t\treturn m_{name};\n'
                output += '\t}\n\n'
            else:
                output += f'\t{prefix}{type}* {name}()const \n'
                output += '\t{\n'
                output += f'\t\treturn m_{name}.get();\n'
                output += '\t}\n\n'

        output += 'private:\n'

        for field in types[expr]:
            type, name = field.split(' ')
            type = get_typename(type, prefix)

            output += f'\t{type} m_{name};\n'

        output += '};\n\n'

    return output

    # with open(outputFile, 'w') as f:
    # f.write(astClasses)


ast = ''
ast += header()

ast += forward_declare(types=expressions, base_class='Expr')
ast += visitor(base_class='Expr', types=expressions)
ast += base_class('Expr')
ast += types(types=expressions)

ast += forward_declare(types=statements, base_class='Stmt')
ast += visitor(base_class='Stmt', types=statements)
ast += base_class('Stmt')
ast += types(types=statements)

filename = '../src/gluon/lang/grammar.h'

with open(filename, 'w') as ast_file:
    ast_file.write(ast)

process = subprocess.Popen(['clang-format', '-i', filename], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
retcode = process.wait()
print(retcode)
