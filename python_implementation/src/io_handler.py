#!/usr/bin/env python3
# encoding: utf-8

"""
Define the IO interface

Author: Barman Roy, Swagato
"""
from abc import ABC, abstractmethod
from typing import List

class AbstractIOHandler(ABC):
    """The abstract IO handler interface."""
    @abstractmethod
    def get_original_lines(self)->List[str]:
        """Get the lines to reverse."""
        raise NotImplementedError
    @abstractmethod
    def persist_reversed_strings(self, lines:List[str])->None:
        """Persist the reversed strings."""
        raise NotImplementedError
class IOHandler(AbstractIOHandler):
    """Subclasses the abstract handler for files stored on disk."""
    def __init__(self, in_file:str, out_file:str):
        """Initialise with the paths."""
        self._in_path_:str=in_file
        self._out_path_:str=out_file

    def get_original_lines(self) ->List[str]:
        """Read the lines from input file."""
        with open(file=self._in_path_) as fp:
            return [*map(str.strip, fp.readlines())]


    def persist_reversed_strings(self, lines:List[str]) ->None:
        """Dump the files in the output path."""
        with open(file=self._out_path_, mode='w') as fp:
            # noinspection PydanticTypeChecker
            [*map(lambda line:print(line, file=fp), lines)]

