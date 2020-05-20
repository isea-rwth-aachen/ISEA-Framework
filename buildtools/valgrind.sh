#!/bin/bash
valgrind --error-exitcode=1 --suppressions=../../valgrind_suppression ./unittestNumeric
