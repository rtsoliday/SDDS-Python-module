# SDDS Python Module

The `sdds` Python module provides a high-level interface for creating, manipulating, and reading **Self Describing Data Sets (SDDS)** files, a file protocol designed for storing and transferring scientific data.

## Features

- **Read and Write** SDDS files in ASCII and binary formats.
- Support for all SDDS data types: parameters, arrays, and columns.
- Define and manipulate data with ease.
- Multi-page data handling for advanced use cases.
- Includes mostly compatible read() and write() functions similar to the PyLHC sdds module. Unlike the PyLHC version, this module works with multi-page SDDS files and with column data.

## Requirements

- Python 3.9 or higher
- An SDDS-compatible environment with the necessary C library dependencies (already integrated into this module).

## Installation

You can install the `sdds` module directly using `pip` or `conda`:

```bash
python -m pip install soliday.sdds
```

```bash
conda install soliday::sdds
```

## Usage

### Creating an SDDS File

Below is an example of how to create an SDDS file with parameters, columns, and arrays.

```python
import sdds

def main():
    # Initialize the SDDS object
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
```

### Reading an SDDS File

Here’s how to read and inspect an SDDS file:

```python
import sdds

def main():
    # Specify the input SDDS file.
    input_file = "output_all_data_types.sdds"
    
    # Load the SDDS file into the SDDS object
    sdds_obj = sdds.load(input_file)
    
    # Determine and display the file mode: Binary or ASCII
    if sdds_obj.mode == sdds.SDDS_BINARY:
        print("SDDS file mode: SDDS_BINARY")
    else:
        print("SDDS file mode: SDDS_ASCII")
    
    # Display the description text if available
    if sdds_obj.description[0]:
        print(f"SDDS file description text: {sdds_obj.description[0]}")
    
    # Display additional description contents if available
    if sdds_obj.description[1]:
        print(f"SDDS file description contents: {sdds_obj.description[1]}")
    
    # Check and print parameter definitions if any are present
    if sdds_obj.parameterName:
        print("\nParameters:")
        for i, definition in enumerate(sdds_obj.parameterDefinition):
            name = sdds_obj.parameterName[i]
            datatype = sdds.sdds_data_type_to_string(definition[4])
            units = definition[1]
            description = definition[2]
            print(f"  {name}")
            print(f"    Datatype: {datatype}", end="")
            if units:
                print(f", Units: {units}", end="")
            if description:
                print(f", Description: {description}", end="")
            print("")  # Newline for readability
    
    # Check and print array definitions if any are present
    if sdds_obj.arrayName:
        print("\nArrays:")
        for i, definition in enumerate(sdds_obj.arrayDefinition):
            name = sdds_obj.arrayName[i]
            datatype = sdds.sdds_data_type_to_string(definition[5])
            units = definition[1]
            description = definition[2]
            dimensions = definition[7]
            print(f"  {name}")
            print(f"    Datatype: {datatype}, Dimensions: {dimensions}", end="")
            if units:
                print(f", Units: {units}", end="")
            if description:
                print(f", Description: {description}", end="")
            print("")  # Newline for readability
    
    # Check and print column definitions if any are present
    if sdds_obj.columnName:
        print("\nColumns:")
        for i, definition in enumerate(sdds_obj.columnDefinition):
            name = sdds_obj.columnName[i]
            datatype = sdds.sdds_data_type_to_string(definition[4])
            units = definition[1]
            description = definition[2]
            print(f"  {name}")
            print(f"    Datatype: {datatype}", end="")
            if units:
                print(f", Units: {units}", end="")
            if description:
                print(f", Description: {description}", end="")
            print("")  # Newline for readability
    
    # Iterate through each loaded page and display parameter, array, and column data
    for page in range(sdds_obj.loaded_pages):
        print(f"\nPage: {page + 1}")
        
        # Display parameter data for the current page
        for i, name in enumerate(sdds_obj.parameterName):
            value = sdds_obj.parameterData[i][page]
            print(f"  Parameter '{name}': {value}")
        
        # Display array data for the current page
        for i, name in enumerate(sdds_obj.arrayName):
            value = sdds_obj.arrayData[i][page]
            print(f"  Array '{name}': {value}")
        
        # Display column data for the current page
        for i, name in enumerate(sdds_obj.columnName):
            value = sdds_obj.columnData[i][page]
            print(f"  Column '{name}': {value}")

    # Opitonally delete the SDDS object
    del sdds_obj

if __name__ == "__main__":
    main()
```

## Documentation

For detailed documentation, examples, and API references, visit the [SDDS Python Module Documentation](https://ops.aps.anl.gov/manuals/SDDSlib/html/files.html).

## License

This project is licensed under the **SDDS Toolkit License**. See the [LICENSE](https://ops.aps.anl.gov/manuals/SDDSlib/html/LICENSE_source.html) file for details.

## Contact

For questions or support, email [soliday@anl.gov](mailto:soliday@anl.gov)
