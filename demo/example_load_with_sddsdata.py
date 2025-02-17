#! /usr/bin/env oagpython3

"""
@file example_load_with_sddsdata.py
@brief Demonstrates reading an SDDS file using the sddsdata module directly.
@details
This script showcases how to utilize the sddsdata module to read and display
the contents of an SDDS (Self Describing Data Set) file. The script performs the
following operations:

1. Initializes an SDDS dataset for reading.
2. Loads an existing SDDS file specified by the user.
3. Determines and displays the file mode (binary or ASCII).
4. Prints the description text and contents if available.
5. Lists all parameters, arrays, and columns along with their metadata.
6. Iterates through each loaded page and displays the data for parameters,
   arrays, and columns.

Usage:
    Ensure that the sddsdata module is installed and the input SDDS file
    (`output_all_data_types.sdds`) is present in the working directory.
"""

import sdds
try:
    from sdds.sddsdata import *
except:
    from sddsdata import *
    
def main():
    fileIndex = 0  # Dataset index for this SDDS file
    input_file = "output_all_data_types.sdds"

    # Initialize SDDS dataset for input
    if InitializeInput(fileIndex, input_file) != 1:
        raise ValueError("Failed to initialize SDDS input file.")

    # Determine and display the file mode: Binary or ASCII
    mode = GetMode(fileIndex)
    if mode == sdds.SDDS_BINARY:
        print("SDDS file mode: SDDS_BINARY")
    else:
        print("SDDS file mode: SDDS_ASCII")

    # Display the description text if available
    description = GetDescription(fileIndex)
    if description[0]:
        print(f"SDDS file description text: {description[0]}")
    if description[1]:
        print(f"SDDS file description contents: {description[1]}")

    # Print parameter definitions
    parameterNames = GetParameterNames(fileIndex)
    if parameterNames:
        print("\nParameters:")
        for name in parameterNames:
            definition = GetParameterDefinition(fileIndex, name)
            print(f"  {name}")
            print(f"    Datatype: {sdds.sdds_data_type_to_string(definition[4])}", end="")
            if definition[1]:
                print(f", Units: {definition[1]}", end="")
            if definition[2]:
                print(f", Description: {definition[2]}", end="")
            print("")

    # Print array definitions
    arrayNames = GetArrayNames(fileIndex)
    if arrayNames:
        print("\nArrays:")
        for name in arrayNames:
            definition = GetArrayDefinition(fileIndex, name)
            print(f"  {name}")
            print(f"    Datatype: {sdds.sdds_data_type_to_string(definition[5])}, Dimensions: {definition[7]}", end="")
            if definition[1]:
                print(f", Units: {definition[1]}", end="")
            if definition[2]:
                print(f", Description: {definition[2]}", end="")
            print("")

    # Print column definitions
    columnNames = GetColumnNames(fileIndex)
    if columnNames:
        print("\nColumns:")
        for name in columnNames:
            definition = GetColumnDefinition(fileIndex, name)
            print(f"  {name}")
            print(f"    Datatype: {sdds.sdds_data_type_to_string(definition[4])}", end="")
            if definition[1]:
                print(f", Units: {definition[1]}", end="")
            if definition[2]:
                print(f", Description: {definition[2]}", end="")
            print("")

    # Read pages and display parameter, array, and column data
    page = ReadPage(fileIndex)
    while page > 0:
        print(f"\nPage: {page}")

        # Display parameter data for the current page
        for name in parameterNames:
            value = GetParameter(fileIndex, name)
            print(f"  Parameter '{name}': {value}")

        # Display array data for the current page
        for name in arrayNames:
            data = GetArray(fileIndex, name)
            dimensions = GetArrayDimensions(fileIndex, name)
            print(f"  Array '{name}': {data}, Dimensions: {dimensions}")

        # Display column data for the current page
        for name in columnNames:
            data = GetColumn(fileIndex, name)
            print(f"  Column '{name}': {data}")

        page = ReadPage(fileIndex)

    # Terminate the SDDS dataset
    if Terminate(fileIndex) != 1:
        raise ValueError("Failed to terminate SDDS dataset.")

if __name__ == "__main__":
    main()
