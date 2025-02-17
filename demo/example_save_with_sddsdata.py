#! /usr/bin/env oagpython3

"""
@file example_save_with_sddsdata.py
@brief Demonstrates writing an SDDS file using the sddsdata module directly.
@details
This script illustrates how to utilize the sddsdata module to create an SDDS (Self Describing Data Set) file. 
The file includes parameters, columns, and arrays covering all supported SDDS data types.

The script performs the following operations:
1. Initializes an SDDS dataset.
2. Defines parameters, columns, and arrays for each supported SDDS data type.
3. Populates the defined parameters, columns, and arrays with sample data across multiple pages.
4. Saves the populated data to an SDDS file.

Supported data types:
    - SDDS_SHORT
    - SDDS_USHORT
    - SDDS_LONG
    - SDDS_ULONG
    - SDDS_LONG64
    - SDDS_ULONG64
    - SDDS_FLOAT
    - SDDS_DOUBLE
    - SDDS_STRING
    - SDDS_CHARACTER
"""

import sdds
try:
    from sdds.sddsdata import *
except:
    from sddsdata import *
import time

def main():
    fileIndex = 0  # Dataset index for this SDDS file
    output_file = "output_all_data_types.sdds"

    # Initialize SDDS output file
    if InitializeOutput(fileIndex, sdds.SDDS_ASCII, 1, "Example output file for demonstration purposes", "Includes parameters, columns, and arrays for every supported datatype.", output_file) != 1:
        raise ValueError("Failed to initialize SDDS output file.")

    # Define parameters
    for name, dtype in [
        ("param_short", sdds.SDDS_SHORT),
        ("param_ushort", sdds.SDDS_USHORT),
        ("param_long", sdds.SDDS_LONG),
        ("param_ulong", sdds.SDDS_ULONG),
        ("param_long64", sdds.SDDS_LONG64),
        ("param_ulong64", sdds.SDDS_ULONG64),
        ("param_float", sdds.SDDS_FLOAT),
        ("param_double", sdds.SDDS_DOUBLE),
        ("param_string", sdds.SDDS_STRING),
        ("param_character", sdds.SDDS_CHARACTER),
    ]:
        if DefineSimpleParameter(fileIndex, name, "", dtype) != 1:
            raise ValueError(f"Failed to define parameter {name}.")

    # Define columns
    for name, dtype in [
        ("col_short", sdds.SDDS_SHORT),
        ("col_ushort", sdds.SDDS_USHORT),
        ("col_long", sdds.SDDS_LONG),
        ("col_ulong", sdds.SDDS_ULONG),
        ("col_long64", sdds.SDDS_LONG64),
        ("col_ulong64", sdds.SDDS_ULONG64),
        ("col_float", sdds.SDDS_FLOAT),
        ("col_double", sdds.SDDS_DOUBLE),
        ("col_string", sdds.SDDS_STRING),
        ("col_character", sdds.SDDS_CHARACTER),
    ]:
        if DefineSimpleColumn(fileIndex, name, "", dtype) != 1:
            raise ValueError(f"Failed to define column {name}.")

    # Define arrays
    for name, dtype, dimensions in [
        ("array_short", sdds.SDDS_SHORT, 1),
        ("array_ushort", sdds.SDDS_USHORT, 1),
        ("array_long", sdds.SDDS_LONG, 2),
        ("array_ulong", sdds.SDDS_ULONG, 1),
        ("array_long64", sdds.SDDS_LONG64, 1),
        ("array_ulong64", sdds.SDDS_ULONG64, 1),
        ("array_float", sdds.SDDS_FLOAT, 1),
        ("array_double", sdds.SDDS_DOUBLE, 1),
        ("array_string", sdds.SDDS_STRING, 1),
        ("array_character", sdds.SDDS_CHARACTER, 1),
    ]:
        if DefineSimpleArray(fileIndex, name, "", dtype, dimensions) != 1:
            raise ValueError(f"Failed to define array {name}.")

    # Write layout
    if WriteLayout(fileIndex) != 1:
        raise ValueError("Failed to write layout.")

    # Populate Page 1
    if StartPage(fileIndex, 2) != 1:
        raise ValueError("Failed to start page 1.")

    # Set parameter values
    for name, data in [
        ("param_short", 1),
        ("param_ushort", 2),
        ("param_long", 3),
        ("param_ulong", 4),
        ("param_long64", 5),
        ("param_ulong64", 6),
        ("param_float", 7.7),
        ("param_double", 8.8),
        ("param_string", "Page 1 String"),
        ("param_character", "A"),
    ]:
        if SetParameter(fileIndex, name, data) != 1:
            raise ValueError(f"Failed to set parameter {name}.")

    # Set column values
    for name, data, in [
        ("col_short", [1, 2]),
        ("col_ushort", [3, 4]),
        ("col_long", [5, 6]),
        ("col_ulong", [7, 8]),
        ("col_long64", [9, 10]),
        ("col_ulong64", [11, 12]),
        ("col_float", [13.1, 14.2]),
        ("col_double", [15.3, 16.4]),
        ("col_string", ["String1", "String2"]),
        ("col_character", ["X", "Y"]),
    ]:
        if SetColumn(fileIndex, name, data) != 1:
            raise ValueError(f"Failed to set column {name}.")

    # Set array values
    for name, data, dims in [
        ("array_short", [1, 2, 3], [3]),
        ("array_ushort", [4, 5, 6], [3]),
        ("array_long", [7, 8, 9, 17, 18, 19], [2, 3]),
        ("array_ulong", [10, 11, 12], [3]),
        ("array_long64", [13, 14, 15], [3]),
        ("array_ulong64", [16, 17, 18], [3]),
        ("array_float", [19.1, 20.2, 21.3], [3]),
        ("array_double", [22.4, 23.5, 24.6], [3]),
        ("array_string", ["Array1", "Array2", "Array3"], [3]),
        ("array_character", ["M", "N", "O"], [3]),
    ]:
        if SetArray(fileIndex, name, data, dims) != 1:
            raise ValueError(f"Failed to set array {name}.")

    # Write page
    if WritePage(fileIndex) != 1:
        raise ValueError("Failed to write page 1.")

    # Populate Page 2
    if StartPage(fileIndex, 2) != 1:
        raise ValueError("Failed to start page 2.")

    # Set parameter values
    for name, data in [
        ("param_short", 10),
        ("param_ushort", 20),
        ("param_long", 30),
        ("param_ulong", 40),
        ("param_long64", 50),
        ("param_ulong64", 60),
        ("param_float", 70.7),
        ("param_double", 80.8),
        ("param_string", "Page 2 String"),
        ("param_character", "B"),
    ]:
        if SetParameter(fileIndex, name, data) != 1:
            raise ValueError(f"Failed to set parameter {name}.")

    # Set column values
    for name, data, in [
        ("col_short", [21, 22]),
        ("col_ushort", [23, 24]),
        ("col_long", [25, 26]),
        ("col_ulong", [27, 28]),
        ("col_long64", [29, 30]),
        ("col_ulong64", [31, 32]),
        ("col_float", [33.1, 34.2]),
        ("col_double", [35.3, 36.4]),
        ("col_string", ["String3", "String4"]),
        ("col_character", ["Z", "W"]),
    ]:
        if SetColumn(fileIndex, name, data) != 1:
            raise ValueError(f"Failed to set column {name}.")

    # Set array values
    for name, data, dims in [
        ("array_short", [101, 102, 103], [3]),
        ("array_ushort", [104, 105, 106], [3]),
        ("array_long", [107, 108, 109, 207, 208, 209], [2, 3]),
        ("array_ulong", [110, 111, 112], [3]),
        ("array_long64", [113, 114, 115], [3]),
        ("array_ulong64", [116, 117, 118], [3]),
        ("array_float", [119.1, 120.2, 121.3], [3]),
        ("array_double", [122.4, 123.5, 124.6], [3]),
        ("array_string", ["Array4", "Array5", "Array6"], [3]),
        ("array_character", ["P", "Q", "R"], [3]),
    ]:
        if SetArray(fileIndex, name, data, dims) != 1:
            raise ValueError(f"Failed to set array {name}.")

    # Write page
    if WritePage(fileIndex) != 1:
        raise ValueError("Failed to write page 1.")

    # Terminate SDDS dataset
    if Terminate(fileIndex) != 1:
        raise ValueError("Failed to terminate SDDS dataset.")

if __name__ == "__main__":
    main()
