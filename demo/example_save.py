#! /usr/bin/env oagpython3

"""
@file example_save.py
@brief Demonstrates writing an SDDS file using the SDDS Python module
@details
This script illustrates how to utilize the SDDS Python module to create an SDDS (Self Describing Data Set) file. 
The file includes parameters, columns, and arrays covering all supported SDDS data types.

The script performs the following operations:
1. Initializes an SDDS object.
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

def main():
    # Initialize a new SDDS object
    sdds_obj = sdds.SDDS()
    
    # Specify the name of the output SDDS file
    output_file = "output_all_data_types.sdds"
    
    # Set the description of the SDDS file with a brief text and detailed contents
    sdds_obj.setDescription(
        "Example output file for demonstration purposes",
        "Includes parameters, columns, and arrays for every supported datatype."
    )
    
    # Define parameters for all supported SDDS data types
    sdds_obj.defineSimpleParameter("param_short",  sdds.SDDS_SHORT)
    sdds_obj.defineSimpleParameter("param_ushort", sdds.SDDS_USHORT)
    sdds_obj.defineSimpleParameter("param_long", sdds.SDDS_LONG)
    sdds_obj.defineSimpleParameter("param_ulong", sdds.SDDS_ULONG)
    sdds_obj.defineSimpleParameter("param_long64", sdds.SDDS_LONG64)
    sdds_obj.defineSimpleParameter("param_ulong64", sdds.SDDS_ULONG64)
    sdds_obj.defineSimpleParameter("param_float", sdds.SDDS_FLOAT)
    sdds_obj.defineSimpleParameter("param_double", sdds.SDDS_DOUBLE)
    sdds_obj.defineSimpleParameter("param_string", sdds.SDDS_STRING)
    sdds_obj.defineSimpleParameter("param_character", sdds.SDDS_CHARACTER)
    
    # Define columns for all supported SDDS data types
    sdds_obj.defineSimpleColumn("col_short", sdds.SDDS_SHORT)
    sdds_obj.defineSimpleColumn("col_ushort", sdds.SDDS_USHORT)
    sdds_obj.defineSimpleColumn("col_long", sdds.SDDS_LONG)
    sdds_obj.defineSimpleColumn("col_ulong", sdds.SDDS_ULONG)
    sdds_obj.defineSimpleColumn("col_long64", sdds.SDDS_LONG64)
    sdds_obj.defineSimpleColumn("col_ulong64", sdds.SDDS_ULONG64)
    sdds_obj.defineSimpleColumn("col_float", sdds.SDDS_FLOAT)
    sdds_obj.defineSimpleColumn("col_double", sdds.SDDS_DOUBLE)
    sdds_obj.defineSimpleColumn("col_string", sdds.SDDS_STRING)
    sdds_obj.defineSimpleColumn("col_character", sdds.SDDS_CHARACTER)
    
    # Define arrays for all supported SDDS data types
    # The third parameter specifies the dimensionality of the array
    sdds_obj.defineSimpleArray("array_short", sdds.SDDS_SHORT, 1)
    sdds_obj.defineSimpleArray("array_ushort", sdds.SDDS_USHORT, 1)
    sdds_obj.defineSimpleArray("array_long", sdds.SDDS_LONG, 2)
    sdds_obj.defineSimpleArray("array_ulong", sdds.SDDS_ULONG, 1)
    sdds_obj.defineSimpleArray("array_long64", sdds.SDDS_LONG64, 1)
    sdds_obj.defineSimpleArray("array_ulong64", sdds.SDDS_ULONG64, 1)
    sdds_obj.defineSimpleArray("array_float", sdds.SDDS_FLOAT, 1)
    sdds_obj.defineSimpleArray("array_double", sdds.SDDS_DOUBLE, 1)
    sdds_obj.defineSimpleArray("array_string", sdds.SDDS_STRING, 1)
    sdds_obj.defineSimpleArray("array_character", sdds.SDDS_CHARACTER, 1)
    
    # -------------------------
    # Populate Page 1 with Data
    # -------------------------
    
    # Set parameter values for Page 1
    sdds_obj.setParameterValue("param_short", 1, page=1)
    sdds_obj.setParameterValue("param_ushort", 2, page=1)
    sdds_obj.setParameterValue("param_long", 3, page=1)
    sdds_obj.setParameterValue("param_ulong", 4, page=1)
    sdds_obj.setParameterValue("param_long64", 5, page=1)
    sdds_obj.setParameterValue("param_ulong64", 6, page=1)
    sdds_obj.setParameterValue("param_float", 7.7, page=1)
    sdds_obj.setParameterValue("param_double", 8.8, page=1)
    sdds_obj.setParameterValue("param_string", "Page 1 String", page=1)
    sdds_obj.setParameterValue("param_character", "A", page=1)
    
    # Define column data for Page 1
    columns_page1 = {
        "col_short": [1, 2],
        "col_ushort": [3, 4],
        "col_long": [5, 6],
        "col_ulong": [7, 8],
        "col_long64": [9, 10],
        "col_ulong64": [11, 12],
        "col_float": [13.1, 14.2],
        "col_double": [15.3, 16.4],
        "col_string": ["String1", "String2"],
        "col_character": ["X", "Y"],
    }
    
    # Populate column data for Page 1
    for col_name, col_data in columns_page1.items():
        sdds_obj.setColumnValueList(col_name, col_data, page=1)
    
    # Define array data for Page 1 (one-dimensional arrays)
    arrays_page1_1d = {
        "array_short": [1, 2, 3],
        "array_ushort": [4, 5, 6],
        "array_ulong": [10, 11, 12],
        "array_long64": [13, 14, 15],
        "array_ulong64": [16, 17, 18],
        "array_float": [19.1, 20.2, 21.3],
        "array_double": [22.4, 23.5, 24.6],
        "array_string": ["Array1", "Array2", "Array3"],
        "array_character": ["M", "N", "O"],
    }
    
    # Populate one-dimensional array data for Page 1
    for array_name, array_data in arrays_page1_1d.items():
        sdds_obj.setArrayValueList(array_name, array_data, [3], page=1)
        
    # Define array data for Page 1 (two-dimensional array)
    arrays_page1_2d = {
        "array_long": [7, 8, 9, 17, 18, 19],
    }
    
    # Populate two-dimensional array data for Page 1
    for array_name, array_data in arrays_page1_2d.items():
        sdds_obj.setArrayValueList(array_name, array_data, [2, 3], page=1)
    
    # -------------------------
    # Populate Page 2 with Data
    # -------------------------
    
    # Set parameter values for Page 2
    sdds_obj.setParameterValue("param_short", 10, page=2)
    sdds_obj.setParameterValue("param_ushort", 20, page=2)
    sdds_obj.setParameterValue("param_long", 30, page=2)
    sdds_obj.setParameterValue("param_ulong", 40, page=2)
    sdds_obj.setParameterValue("param_long64", 50, page=2)
    sdds_obj.setParameterValue("param_ulong64", 60, page=2)
    sdds_obj.setParameterValue("param_float", 70.7, page=2)
    sdds_obj.setParameterValue("param_double", 80.8, page=2)
    sdds_obj.setParameterValue("param_string", "Page 2 String", page=2)
    sdds_obj.setParameterValue("param_character", "B", page=2)
    
    # Define column data for Page 2
    columns_page2 = {
        "col_short": [21, 22],
        "col_ushort": [23, 24],
        "col_long": [25, 26],
        "col_ulong": [27, 28],
        "col_long64": [29, 30],
        "col_ulong64": [31, 32],
        "col_float": [33.1, 34.2],
        "col_double": [35.3, 36.4],
        "col_string": ["String3", "String4"],
        "col_character": ["Z", "W"],
    }
    
    # Populate column data for Page 2
    for col_name, col_data in columns_page2.items():
        sdds_obj.setColumnValueList(col_name, col_data, page=2)
    
    # Define array data for Page 2 (one-dimensional arrays)
    arrays_page2_1d = {
        "array_short": [101, 102, 103],
        "array_ushort": [104, 105, 106],
        "array_ulong": [110, 111, 112],
        "array_long64": [113, 114, 115],
        "array_ulong64": [116, 117, 118],
        "array_float": [119.1, 120.2, 121.3],
        "array_double": [122.4, 123.5, 124.6],
        "array_string": ["Array4", "Array5", "Array6"],
        "array_character": ["P", "Q", "R"],
    }
    
    # Populate one-dimensional array data for Page 2
    for array_name, array_data in arrays_page2_1d.items():
        sdds_obj.setArrayValueList(array_name, array_data, [3], page=2)
    
    # Define array data for Page 2 (two-dimensional array)
    arrays_page2_2d = {
        "array_long": [107, 108, 109, 207, 208, 209],
    }
    
    # Populate two-dimensional array data for Page 2
    for array_name, array_data in arrays_page2_2d.items():
        sdds_obj.setArrayValueList(array_name, array_data, [2, 3], page=2)
    
    # -------------------------
    # Save the SDDS File
    # -------------------------
    
    # Save all the defined data into the specified SDDS file
    sdds_obj.save(output_file)

    # Opitonally delete the SDDS object
    del sdds_obj

if __name__ == "__main__":
    main()
