#!/usr/bin/env python3
# encoding: utf-8

"""
Define the string manipulation

Author: Barman Roy, Swagato
"""
from abc import ABC, abstractmethod
from typing import List


class AbstractManipulator(ABC):
    """Define abstract behaviour of the reversal agent."""

    @abstractmethod
    def get_reversed_strings(self) -> List[str]:
        """Get the reversed strings."""
        raise NotImplementedError


class Reverser(AbstractManipulator):
    """Implement the reverser class."""

    def __init__(self, originals: List[str]):
        """Store the originals."""
        self._originals_: List[str] = originals

    def get_reversed_strings(self) -> List[str]:
        """Get the reversed strings."""
        return [*map(lambda sample: sample[::-1], self._originals_)]
