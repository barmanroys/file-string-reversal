#!/usr/bin/env python3
# encoding: utf-8

"""
Define the entry point and orchestrator

Author: Barman Roy, Swagato
"""
from io_handler import AbstractIOHandler, List, IOHandler
from string_reversal import AbstractManipulator, Reverser


class Main:
    """The main job agent."""

    def __init__(self, handler: AbstractIOHandler):
        """Define the input and output."""
        self._handler_: AbstractIOHandler = handler

    def run(self) -> None:
        """Run the job."""
        lines: List[str] = self._handler_.get_original_lines()
        manipulator: AbstractManipulator = Reverser(originals=lines)
        outs: List[str] = manipulator.get_reversed_strings()
        self._handler_.persist_reversed_strings(lines=outs)


if __name__ == '__main__':
    in_path: str = '../../data/in_file.txt'
    out_path: str = '../../data/out_file.txt'
    test_handler: AbstractIOHandler = IOHandler(in_file=in_path, out_file=out_path)
    Main(handler=test_handler).run()
