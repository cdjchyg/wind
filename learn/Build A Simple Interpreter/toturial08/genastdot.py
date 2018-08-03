
#https://ruslanspivak.com/lsbasi-part7/
#python genastdot.py "- 3" > ast.dot && dot -Tpng -o ast.png ast.dot

import argparse
import textwrap

from spi import Lexer, Parser, NodeVisitor

class ASTVisualizer(NodeVisitor):
    def __init__(self, parser):
        self.parser = parser
        self.ncount = 1
        self.dot_header = [textwrap.dedent("""\
        digraph astgrah {
          node [shape=circle, fontsize = 12, fontname="Courier", height=.1];
          ranksep=.3;
          edge [arrowsize=.5]
        """)]
        self.dot_body = []
        self.dot_footer = ['}']

    def visit_Num(self, node):
        s = '  node{} [label="{}"]\n'.format(self.ncount, node.token.value)
        self.dot_body.append(s)
        node._num = self.ncount
        self.ncount += 1

    def visit_BinOp(self, node):
        s = '  node{} [label="{}"]\n'.format(self.ncount, node.op.value)
        self.dot_body.append(s)
        node._num = self.ncount
        self.ncount += 1

        self.visit(node.left)
        self.visit(node.right)

        for child_node in (node.left, node.right):
            s = '  node{} -> node{}\n'.format(node._num, child_node._num)
            self.dot_body.append(s)
    
    def visit_UnaryOp(self, node):
        s = '  node{} [label="unary {}"]\n'.format(self.ncount, node.op.value)
        self.dot_body.append(s)
        node._num = self.ncount
        self.ncount += 1

        self.visit(node.expr)
        s = '  node{} -> node{}\n'.format(node._num, node.expr._num)
        self.dot_body.append(s)

    def gendot(self):
        tree = self.parser.parse()
        self.visit(tree)
        return ''.join(self.dot_header + self.dot_body + self.dot_footer)

def main():
    argparser = argparse.ArgumentParser(
        description='Generate a Parse Tree DOT file.'
    )

    argparser.add_argument(
        'text',
        help='Arithmetic expression(in quotes):"1 + 2 * 3"'
    )

    args = argparser.parse_args()
    text = args.text
    #text = '14 + 2 * 3 - 6 / 2'
    lexer = Lexer(text)
    parser = Parser(lexer)

    viz = ASTVisualizer(parser)
    content = viz.gendot()
    print(content)

if __name__ == '__main__':
    main()